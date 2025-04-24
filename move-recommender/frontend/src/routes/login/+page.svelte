<script lang="ts">
  import { goto } from "$app/navigation";
  import { fetchApi, userJwt } from "$lib/api.svelte";

  if ($userJwt) {
    goto("/");
  }

  let err = $state();
  let username = $state();
  let password = $state();
  let loading = $state(false);
</script>

<h1>Login</h1>
<p>Login to your account</p>

<form
  onsubmit={async () => {
    loading = true;

    err = undefined;
    const response = await fetchApi<any>("/token", {
      method: "POST",
      headers: {
        "Content-Type": "application/x-www-form-urlencoded",
        // Authorization: "Basic Og==",
      },
      body: `grant_type=password&username=${username}&password=${password}`,
    });

    console.log("here");

    if (!response.success) {
      err = response.message;
      loading = false;
      return;
    }

    userJwt.set(response.response.access_token);

    goto("/");

    loading = false;
  }}
>
  <label for="username">Username</label>
  <input
    type="text"
    id="username"
    bind:value={username}
    required
    placeholder="Enter your username"
  />
  <label for="password">Password</label>
  <input
    type="password"
    id="password"
    bind:value={password}
    required
    placeholder="Enter your password"
  />

  <button type="submit" disabled={loading}>Login</button>

  {#if err}
    <p style="color: red;">{err}</p>
  {/if}
</form>
