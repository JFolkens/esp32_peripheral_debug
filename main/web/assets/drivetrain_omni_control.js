(DRIVETRAIN_NAME) => {
    const stop_button = document.getElementById(DRIVETRAIN_NAME + '_stop_button');
    const y_speed = document.getElementById(DRIVETRAIN_NAME + '_y_speed_slider');
    const x_speed = document.getElementById(DRIVETRAIN_NAME + '_x_speed_slider');
    const a_speed = document.getElementById(DRIVETRAIN_NAME + '_a_speed_slider');

    stop_button.addEventListener('click', async() => {
        const response = await fetch('/' + DRIVETRAIN_NAME + '/update?stop=true', { method: 'POST' });
        if (response.ok) {
            document.getElementById(DRIVETRAIN_NAME + '_state').innerHTML = await response.text();
        }
        // Set sliders to stopped positions
        y_speed.value = 0;
        x_speed.value = 0;
        a_speed.value = 0;
    });

    let debounceTimer = null;
    const USER_THROTTLE_MS = 300;

    async function sendValue() {
        const x = x_speed.value;
        const response = await fetch(
            '/' + DRIVETRAIN_NAME + '/update?x=' + encodeURIComponent(x_speed.value)
            + '&y=' + encodeURIComponent(value) + '&a=' + encodeURIComponent(a_speed.value),
            { method: 'POST' });
        if (response.ok) {
            document.getElementById(DRIVETRAIN_NAME + '_state').innerHTML = await response.text();
        }
    }

    [y_speed, x_speed, a_speed].forEach((slider) => {
        slider.addEventListener('input', (e) => {
            if (debounceTimer) clearTimeout(debounceTimer);
            debounceTimer = setTimeout(() => {
                debounceTimer = null;
                sendValue();
            }, USER_THROTTLE_MS);
        });

        slider.addEventListener('change', (e) => {
            if (debounceTimer) {
                clearTimeout(debounceTimer);
                debounceTimer = null;
            }
            sendValue();
        });
    });

}
