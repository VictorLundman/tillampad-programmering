<script lang="ts">
  import { goto } from "$app/navigation";
  import { fetchApi, userJwt } from "$lib/api.svelte";
  import RateMovie from "./rateMovie.svelte";

  if (!$userJwt) {
    goto("/login");
  }

  let predicted: Record<number, string> | undefined = $state();
  let loading = $state(false);
  let err = $state();
</script>

{$userJwt}

<RateMovie />

<button
  onclick={async () => {
    loading = true;
    err = undefined;
    const response = await fetchApi<any>("/users/me/predict/", {
      method: "GET",
    });

    if (!response.success) {
      err = response.message;
      loading = false;
      return;
    }

    predicted = response.response;
    loading = false;
  }}
  disabled={loading}>Get predictions</button
>

{#if predicted}
  <ol>
    {#each Object.entries(predicted) as [_, title]}
      <li>{title}</li>
    {/each}
  </ol>
{:else}
  <p>Click load predictions!</p>
{/if}

{#if err}
  <p style="color: red;">{err}</p>
{/if}
