# Movie recommender

En filmrekommenderare som använder colaborative filtering för att rekommendera nya filmer till användare. Baserat på de filmer användaren har recenserat och hur hen har recenserat dem hittar algoritmen x antal liknande användare. Sedan plockar algoritmen ut de filmerna med högst genomsnittligt betyg bland de liknande användarna. Av de filmerna rekomenderas användaren de som hen inte redan har betygsatt. 

Skaffa data från movielens. Lägg i data mappen. Det som behövs är movies och ratings. 

```
python -m venv .venv
source .venv/Scripts/activate
python -m pip install -r requirements.txt
```

Välj sätt py interpreter i vs-code

Kör main.ipynb

```
python -m fastapi dev app.py
```

```
cd frontend
npm i
npm run dev
```

Skapa användarkonton på http://127.0.0.1:8000/docs hemsidan är på http://localhost:5173/.