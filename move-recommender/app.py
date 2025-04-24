from datetime import datetime, timedelta, timezone
from typing import Annotated

import jwt
from fastapi import Depends, FastAPI, HTTPException, status, Query
from fastapi.security import OAuth2PasswordBearer, OAuth2PasswordRequestForm
from jwt.exceptions import InvalidTokenError
from passlib.context import CryptContext
from pydantic import BaseModel
from env import SECRET_KEY
from sqlmodel import Field, SQLModel, create_engine, Session, select, Relationship
import pandas as pd
from rapidfuzz import fuzz, process
import pickle
import itertools
from fastapi.middleware.cors import CORSMiddleware

origins = [
    "http://localhost:5173",
]

movies: pd.DataFrame = pd.read_pickle("movies.pkl")
movie_ratings: pd.DataFrame = pd.read_pickle("ratings.pkl")
movie_names = movies["title"].to_list()
movie_ids = movies.index.to_list()

with open('model.pkl', 'rb') as f:
    perdiction_model = pickle.load(f)

ALGORITHM = "HS256"
ACCESS_TOKEN_EXPIRE_MINUTES = 30

class Token(BaseModel):
    access_token: str
    token_type: str

class SignUp(BaseModel):
    username: Annotated[str, Query(min_length=2, max_length=20)]
    password: Annotated[str, Query(min_length=8, max_length=100)]

class TokenData(BaseModel):
    username: str | None = None

class User(SQLModel, table=True):
    id: int | None = Field(default=None, primary_key=True)
    username: str = Field(index=True, unique=True)
    disabled: bool | None = None
    hashed_password: str = Field()
    
    reviews: list["Review"] = Relationship(back_populates="user")

class Review(SQLModel, table=True):
    id: int | None = Field(default=None, primary_key=True)
    movie_id: int = Field()
    rating: int = Field()

    user_id: int = Field(index=True, foreign_key="user.id")
    user: User = Relationship(back_populates="reviews")
    
sqlite_file_name = "database.db"
sqlite_url = f"sqlite:///{sqlite_file_name}"

connect_args = {"check_same_thread": False}
engine = create_engine(sqlite_url, connect_args=connect_args)

def create_db_and_tables():
    SQLModel.metadata.create_all(engine)

def get_session():
    with Session(engine) as session:
        yield session

SessionDep = Annotated[Session, Depends(get_session)]

pwd_context = CryptContext(schemes=["bcrypt"], deprecated="auto")

oauth2_scheme = OAuth2PasswordBearer(tokenUrl="token")

app = FastAPI()

app.add_middleware(
    CORSMiddleware,
    allow_origins=origins,
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

@app.on_event("startup")
def on_startup():
    create_db_and_tables()

def verify_password(plain_password, hashed_password):
    return pwd_context.verify(plain_password, hashed_password)


def get_password_hash(password):
    return pwd_context.hash(password)


def get_user(session: SessionDep, username: str):
    user = session.exec(select(User).where(User.username == username)).first()
    return user


def authenticate_user(session: SessionDep, username: str, password: str):
    user = get_user(session, username)
    if not user:
        return False
    if not verify_password(password, user.hashed_password):
        return False
    return user

def create_user(session: SessionDep, username: str, password: str):
    session.add(User(username=username, hashed_password=get_password_hash(password)))
    session.commit()
    return get_user(session, username)

def create_access_token(data: dict, expires_delta: timedelta | None = None):
    to_encode = data.copy()
    if expires_delta:
        expire = datetime.now(timezone.utc) + expires_delta
    else:
        expire = datetime.now(timezone.utc) + timedelta(minutes=15)
    to_encode.update({"exp": expire})
    encoded_jwt = jwt.encode(to_encode, SECRET_KEY, algorithm=ALGORITHM)
    return encoded_jwt


async def get_current_user(session: SessionDep, token: Annotated[str, Depends(oauth2_scheme)]):
    credentials_exception = HTTPException(
        status_code=status.HTTP_401_UNAUTHORIZED,
        detail="Could not validate credentials",
        headers={"WWW-Authenticate": "Bearer"},
    )
    try:
        payload = jwt.decode(token, SECRET_KEY, algorithms=[ALGORITHM])
        username = payload.get("sub")
        if username is None:
            raise credentials_exception
        token_data = TokenData(username=username)
    except InvalidTokenError:
        raise credentials_exception
    user = get_user(session, username=token_data.username)
    if user is None:
        raise credentials_exception
    return user


async def get_current_active_user(
    current_user: Annotated[User, Depends(get_current_user)],
):
    if current_user.disabled:
        raise HTTPException(status_code=400, detail="Inactive user")
    return current_user


@app.post("/token")
async def login_for_access_token(
    session: SessionDep,
    form_data: Annotated[OAuth2PasswordRequestForm, Depends()],
) -> Token:
    user = authenticate_user(session, form_data.username, form_data.password)
    if not user:
        raise HTTPException(
            status_code=status.HTTP_401_UNAUTHORIZED,
            detail="Incorrect username or password",
            headers={"WWW-Authenticate": "Bearer"},
        )
    access_token_expires = timedelta(minutes=ACCESS_TOKEN_EXPIRE_MINUTES)
    access_token = create_access_token(
        data={"sub": user.username}, expires_delta=access_token_expires
    )
    return Token(access_token=access_token, token_type="bearer")

@app.post("/signup")
def async_signup(session: SessionDep, form_data: Annotated[SignUp, Depends()]):
    user = get_user(session, form_data.username)
    if user:
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail="Username already in use"
        )

    user = create_user(session, form_data.username, form_data.password)
    
    return {"success": True, "username": user.username}


@app.get("/users/me/reviews/")
async def read_own_items(
    current_user: Annotated[User, Depends(get_current_active_user)],
):
    return current_user.reviews

@app.post("/users/me/reviews/")
async def rate_movie(
    session: SessionDep,
    current_user: Annotated[User, Depends(get_current_active_user)],
    movie_id: int,
    rating: int
):
    if not 0 <= rating <= 5:
        raise HTTPException(status.HTTP_400_BAD_REQUEST, "Invalid rating")

    if movie_id not in movie_ids:
        raise HTTPException(status.HTTP_400_BAD_REQUEST, "Invalid movie id")

    existing_rating = session.exec(select(Review).where(Review.user_id == current_user.id, Review.movie_id == movie_id)).first()
    if existing_rating:
        raise HTTPException(status.HTTP_400_BAD_REQUEST, "Movie already rated")
    
    session.add(Review(movie_id=movie_id, user_id=current_user.id, rating=rating))
    session.commit()

    return {
        "id": movie_id,
        "rating": rating
    }

def ratings_dict_to_list(ratings_dict: dict[int, int]) -> pd.DataFrame:
    ratings_df = pd.DataFrame(list(ratings_dict.items()), columns=["movieId", "rating"]).fillna(0)

    ratings_df.set_index('movieId', inplace=True)
    ratings_df = ratings_df.reindex(movie_ids, fill_value=0)

    return ratings_df["rating"].to_list()

@app.get("/users/me/predict/")
async def predict_rating(
    session: SessionDep,
    current_user: Annotated[User, Depends(get_current_active_user)],
):
    ratings = session.exec(select(Review).where(Review.user_id == current_user.id)).all()
    if not ratings or len(ratings) == 0:
        raise HTTPException(status.HTTP_404_NOT_FOUND, "No ratings found")
    
    print(len(ratings))

    ratings_dict = { rating.id: rating.rating for rating in ratings }
    ratings_list = ratings_dict_to_list(ratings_dict)
    distances, indices = perdiction_model.kneighbors([ratings_list])

    nearest_users = indices[0]
    print(nearest_users)
    top_movies = movie_ratings.iloc[nearest_users].mean().sort_values(ascending=False)  # Find highest-rated movies
    recommended_movies = itertools.islice(filter(lambda x : x not in ratings_dict.keys(), top_movies.index.tolist()), 10)

    # print(recommended_movies)

    found_movies = {movie_id: movies.loc[movie_id, "title"] for movie_id in recommended_movies}

    return found_movies

@app.get("/movies")
async def search_movies(
    current_user: Annotated[User, Depends(get_current_active_user)],
    query: str
):
    matches = process.extract(
        query, movie_names, scorer=fuzz.token_sort_ratio, limit=10
    )

    results = []
    for name, score, _ in matches:
        movie_data = movies[movies["title"] == name].iloc[0]
        movie_id = int(movie_data.name)
        print(movie_id)
        movie_data = movie_data.to_dict()
        
        movie_data["score"] = score
        movie_data["id"] = movie_id
        print(movie_data)
        results.append(movie_data)

    return results