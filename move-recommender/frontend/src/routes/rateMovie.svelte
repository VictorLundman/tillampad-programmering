<script lang="ts">
  import { fetchApi } from "$lib/api.svelte";

  let err: string | undefined = $state();
  let loading = $state(false);
  let searchQuery: string = $state("");
  let searchResults:
    | {
        title: string;
        id: number;
      }[]
    | undefined = $state();
  let selectedMovieId: number | undefined = $state();
  let rating: number | undefined = $state();
  let ratingSuccess = $state(false);
</script>

<form
  onsubmit={async () => {
    loading = true;
    ratingSuccess = false;
    err = undefined;

    const response = await fetchApi<
      {
        title: string;
        id: number;
      }[]
    >(`/movies?query=${encodeURI(searchQuery)}`, {
      method: "GET",
    });

    if (!response.success) {
      err = response.message;
      loading = false;
      return;
    }

    console.log(response.response);
    searchResults = response.response;
    loading = false;
  }}
>
  <label for="search">Search</label>
  <input
    type="text"
    id="search"
    bind:value={searchQuery}
    required
    placeholder="Enter your search query"
  />
  <button type="submit" disabled={loading}>Search</button>
</form>

{#if searchResults}
  <form
    onsubmit={async () => {
      loading = true;
      err = undefined;

      const response = await fetchApi<any>(
        `/users/me/reviews?movie_id=${selectedMovieId}&rating=${rating}`,
        {
          method: "POST",
        }
      );

      if (!response.success) {
        err = response.message;
        loading = false;
        return;
      }

      console.log(response.response);
      ratingSuccess = true;
      loading = false;
      searchResults = undefined;
      selectedMovieId = undefined;
      rating = undefined;
    }}
  >
    {#each searchResults as movie}
      <div>
        <input
          type="radio"
          name="movie"
          id={movie.id.toString()}
          value={movie.id}
          bind:group={selectedMovieId}
        />
        <label for={movie.id.toString()}>{movie.title}</label>
      </div>
    {/each}
    <label for="rating">Rating</label>
    <input
      type="number"
      id="rating"
      bind:value={rating}
      required
      min="1"
      max="5"
    />

    <button type="submit">Rate</button>
  </form>
{:else}
  <p>Search!</p>
  {#if ratingSuccess}
    <p>Successfully rated movie</p>
  {/if}
{/if}

{#if err}
  <p style="color: red;">{err}</p>
{/if}
