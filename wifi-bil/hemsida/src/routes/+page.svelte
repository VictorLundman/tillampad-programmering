<script lang="ts">
	import ConnectionForm from './connectionForm.svelte';

	let ip = $state<string | null>(null);

	let left = $state(false);
	let right = $state(false);
	let forward = $state(false);

	async function updateMotors() {
		if (!ip) return;

		const data = forward ? '11' : left ? '10' : right ? '01' : '00';

		console.log('Setting', data);
		const response = await fetch(`http://${ip}/`, {
			headers: {
				'x-motor-data': data
			}
		});
		const json = await response.json();

		console.log('response', json);

		left = json.left && !json.right;
		right = json.right && !json.left;
		forward = json.left && json.right;
	}
</script>

{#if !ip}
	<ConnectionForm
		onConnect={(_ip) => {
			ip = _ip;
		}}
	/>
{:else}
	<button
		style:color={left ? 'red' : 'black'}
		onmousedown={() => {
			left = true;
			updateMotors();
		}}
		onmouseup={() => {
			left = false;
			updateMotors();
		}}>Left</button
	>
	<button
		style:color={forward ? 'red' : 'black'}
		onmousedown={() => {
			forward = true;
			updateMotors();
		}}
		onmouseup={() => {
			forward = false;
			updateMotors();
		}}>Forward</button
	>
	<button
		style:color={right ? 'red' : 'black'}
		onmousedown={() => {
			right = true;
			updateMotors();
		}}
		onmouseup={() => {
			right = false;
			updateMotors();
		}}>Right</button
	>
{/if}

<a
	href="https://github.com/VictorLundman/tillampad-programmering/tree/main/wifi-bil"
	target="_blank">Github</a
>
