<script lang="ts">
	import ConnectionForm from './connectionForm.svelte';

	let webSocket = $state<WebSocket | null>(null);

	let left = $state(false);
	let right = $state(false);
	let forward = $state(false);

	type MotorState = 'Forward' | 'Reverse' | 'Off';

	let leftMotor = $state<MotorState>('Off');
	let rightMotor = $state<MotorState>('Off');

	function updateMotors() {
		if (!webSocket) return;

		let op = 0b00000000;
		if (forward) {
			op = 0b00111100;
		} else if (left) {
			op = 0b00101100;
		} else if (right) {
			op = 0b00111000;
		}

		const payload = new Uint8Array();
		payload[0] = op;

		console.log(payload);

		webSocket.send(payload);
	}

	function onData(event: MessageEvent<Uint8Array>) {
		const data = event.data[0];
		const opCode = (data & 0b11000000) >> 6;

		console.log('got data', data.toString(2), opCode);

		if (opCode == 0b00000000) {
			const leftMotorDirection = (data & 0b00100000) >> 5;
			const leftMotorStatus = (data & 0b00010000) >> 4;

			const rightMotorDirection = (data & 0b00001000) >> 3;
			const rightMotorStatus = (data & 0b00000100) >> 2;

			leftMotor = !leftMotorStatus ? 'Off' : leftMotorDirection ? 'Forward' : 'Reverse';
			rightMotor = !rightMotorStatus ? 'Off' : rightMotorDirection ? 'Forward' : 'Reverse';
		}
	}
</script>

{#if !webSocket}
	<ConnectionForm
		onConnect={(socket) => {
			webSocket = socket;
			webSocket.onmessage = onData;
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
