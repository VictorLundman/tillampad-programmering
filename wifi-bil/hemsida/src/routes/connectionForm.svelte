<script lang="ts">
	let {
		onConnect
	}: {
		onConnect: (connection: WebSocket) => void;
	} = $props();

	let connecting = $state(false);
	let connected = $state(false);
	let error = $state<string | null>(null);
	let ip = $state<string>('127.');

	function connect(event: SubmitEvent) {
		event.preventDefault();

		connecting = true;

		let webSocket: WebSocket;

		try {
			webSocket = new WebSocket(ip);
		} catch (err) {
			console.log('Error while connecting to socket!');
			console.error(err);

			connected = false;
			connecting = false;
			error = 'Unable to connect to socket (check terminal for more info)';

			return;
		}

		webSocket.onopen = (event) => {
			connected = true;
			connecting = false;

			onConnect(webSocket);
		};
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
