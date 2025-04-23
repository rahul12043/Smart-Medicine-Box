function updateData() {
    fetch('/latest')
        .then(response => response.json())
        .then(data => {
            document.getElementById('sensor_time').textContent = data.time ?? '--';
            document.getElementById('temp').textContent = data.temperature ?? '--';
            document.getElementById('hum').textContent = data.humidity ?? '--';
            document.getElementById('ldr').textContent = data.ldr ?? '--';
            document.getElementById('ir').textContent = data.ir ?? '--';
            document.getElementById('distance').textContent = data.distance ?? '--';
            document.getElementById('time').textContent = data.timestamp ?? '--';
        })
        .catch(err => console.error('Error fetching data:', err));
}

// Fetch data every 2 seconds
setInterval(updateData, 2000);
updateData(); // Initial call
