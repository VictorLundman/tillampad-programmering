<script lang="ts">
	let {
		onConnect
	}: {
		onConnect: (ip: string) => void;
	} = $props();

	let connecting = $state(false);
	let connected = $state(false);
	let error = $state<string | null>(null);
	let ip = $state<string>('192.');

	async function connect(event: SubmitEvent) {
		event.preventDefault();

		connecting = true;

		console.log('Trying to connect!');

		try {
			const data = await fetch(`http://${ip}/`);
			const dataJson = await data.json();
			console.log(dataJson);
		} catch (err) {
			console.log('Error while connecting!');
			console.error(err);

			connected = false;
			connecting = false;
			error = 'Unable to connect (check terminal for more info)';

			return;
		}

		onConnect(ip);
	}
</script>

<form onsubmit={connect}>
	<label for="ip">Local ip address: </label>
	<input name="ip" type="text" bind:value={ip} disabled={connecting || connected} />
	<button disabled={connecting || connected}>{connected ? 'Already connected' : 'Connect'}</button>
	{#if error}
		<p class="error">{error}</p>
	{/if}
</form>

<style>
	.error {
		color: red;
	}
</style>
