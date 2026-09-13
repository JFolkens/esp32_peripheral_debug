(LED_NAME) => {
    const button = document.getElementById(LED_NAME + '_button');
    button.addEventListener('click', async () => {
        const action = button.dataset.action;
        const response = await fetch('/' + LED_NAME + '/update?action=' + action, { method: 'POST' });
        if (response.ok) {
            document.getElementById(LED_NAME + '_state').innerHTML = await response.text();
        }
        const isOn = action === 'on';
        button.dataset.action = isOn ? 'off' : 'on';
        button.textContent = isOn ? 'Turn OFF' : 'Turn ON';
        button.className = isOn ? 'button' : 'button button-off';
    });
}
