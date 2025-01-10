<script lang="ts">
	import ConnectionForm from './connectionForm.svelte';

	let ip = $state<string | null>(null);

	let left = $state(false);
	let right = $state(false);
	let forward = $state(false);

	type MotorState = 'Forward' | 'Reverse' | 'Off';

	let leftMotor = $state<MotorState>('Off');
	let rightMotor = $state<MotorState>('Off');

	async function updateMotors() {
		if (!ip) return;
		const response = await fetch(ip, {
			headers: {
				data: (leftMotor ? '1' : '0') + (rightMotor ? '1' : '0')
			}
		});
		const json = await response.json();

		leftMotor = json.left == '1' ? 'Forward' : 'Off';
		rightMotor = json.right == '1' ? 'Forward' : 'Off';
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
		style:color={right ? 'red' : 'black'}
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
		style:color={right ? 'red' : 'black'}
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

	<p>Left motor: {leftMotor} | Right motor: {rightMotor}</p>
{/if}
