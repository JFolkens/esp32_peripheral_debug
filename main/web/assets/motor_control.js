(MOTOR_NAME) => {
    const slider = document.getElementById(MOTOR_NAME + '_slider');
    const display = document.getElementById(MOTOR_NAME + '_display');
    const directions = document.querySelectorAll('input[name="' + MOTOR_NAME + '_direction"]');

    let debounceTimer = null;
    const USER_THROTTLE_MS = 300;

    function selectedDirection() {
        return document.querySelector('input[name="' + MOTOR_NAME + '_direction"]:checked').value;
    }

    async function sendValue(direction, value) {
        const response = await fetch(
            '/' + MOTOR_NAME + '/update?mode=' + encodeURIComponent(direction) + '&speed=' + encodeURIComponent(value),
            { method: 'POST' });
        if (response.ok) {
            document.getElementById(MOTOR_NAME + '_state').innerHTML = await response.text();
        }
    }

    slider.addEventListener('input', (e) => {
        const value = e.target.value;
        display.textContent = value;
        if (debounceTimer) clearTimeout(debounceTimer);
        debounceTimer = setTimeout(() => {
            debounceTimer = null;
            sendValue(selectedDirection(), value);
        }, USER_THROTTLE_MS);
    });

    slider.addEventListener('change', (e) => {
        if (debounceTimer) {
            clearTimeout(debounceTimer);
            debounceTimer = null;
        }
        sendValue(selectedDirection(), e.target.value);
    });

    directions.forEach((direction) => {
        direction.addEventListener('change', (e) => {
            if (debounceTimer) {
                clearTimeout(debounceTimer);
                debounceTimer = null;
            }
            const off = e.target.value === 'off';
            slider.disabled = off;
            if (off) display.textContent = '0';
            sendValue(e.target.value, off ? 0 : slider.value);
        });
    });
}
