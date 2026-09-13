(PWM_NAME) => {
    const slider = document.getElementById(PWM_NAME + '_slider');
    const display = document.getElementById(PWM_NAME + '_display');

    let debounceTimer = null;
    const USER_THROTTLE_MS = 300;

    async function sendValue(val) {
        const response = await fetch(
            '/' + PWM_NAME + '/update?speed=' + encodeURIComponent(val),
            { method: 'POST' });
        if (response.ok) {
            document.getElementById(PWM_NAME + '_state').innerHTML = await response.text();
        }
    }

    slider.addEventListener('input', (e) => {
        const val = e.target.value;
        display.textContent = val;
        if (debounceTimer) clearTimeout(debounceTimer);
        debounceTimer = setTimeout(() => {
            debounceTimer = null;
            sendValue(val);
        }, USER_THROTTLE_MS);
    });

    slider.addEventListener('change', (e) => {
        if (debounceTimer) {
            clearTimeout(debounceTimer);
            debounceTimer = null;
        }
        sendValue(e.target.value);
    });
};
