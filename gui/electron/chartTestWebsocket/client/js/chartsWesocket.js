 $(function(){
 	$(document).ready(function() {
		function addDataToChart(chart,data){
        	chart.data.labels.push('');
                chart.data.datasets[0].data.push(data);
                if(chart.data.labels.length > 50) {
                    chart.data.labels.shift();
                    chart.data.datasets[0].data.shift();
                }
                chart.update();
            };

            let ctx = document.getElementById('flowChart').getContext('2d');

            let chartOptions = {
                    animation: { duration: 1 },       
                    hover: { animationDuration: 0 },
                    responsiveAnimationDuration: 0,
                    tooltips:  { enabled: false },
                    hover:     { animationDuration: 0 },
                    scales:    { y: { type: 'linear',min: -50, max: 100 } },
                    legend:    { display: false },
                    title:     { display: false, text: '' },
                    responsive: true,
                    elements: { line: { tension: 0 }, point:{radius:0}},
                    scales: {
                        xAxes: [{
                            gridLines: {
                                display: true,
                                drawBorder: true,
                                drawOnChartArea: false,
                            },
                            display: true,
                                scaleLabel: {
                                display: false,
                                labelString: ''
                            }
                        }],
                        yAxes: [{
                            gridLines: {
                                display: true,
                                drawBorder: true,
                                drawOnChartArea: false,
                            },
                            display: true,
                            label: {
                                display: false,
                                labelString: ''
                            }
                        }]
                    }
                };


                let flowChart = new Chart(ctx, {
                    type: 'line',
                    data: {
                        labels: ['0'],
                        datasets: [{
                            data: [],
                            backgroundColor: [ 'rgba(255, 255, 255, 0.2)' ],
                            borderColor: [ '#1EB7FF' ],
                            borderWidth: 1
                        }]
                    },
                    options: chartOptions
                });


            ctx = document.getElementById('pressureChart').getContext('2d');

            let pressureChart = new Chart(ctx, {
                type: 'line',
                data: {
                    labels: ['0'],
                    datasets: [{
                        data: [],
                            backgroundColor: [ 'rgba(255, 255, 255, 0.2)' ],
                            borderColor: [ '#1EB7FF' ],
                            borderWidth: 1
                    }]
                },
                options: chartOptions
            });

            var socket = io.connect('http://localhost:8081');
              socket.on('serialPortData', function (data) {
                let rawCommand = data.data.split(':');
                let commandID  = rawCommand[0];
                if ( commandID === 'DATA'){
                     let ventilatorData = rawCommand[1].split(',');
                     if (ventilatorData.length > 2){
                        let flow     = parseFloat(ventilatorData[0]);
                        let pressure = parseFloat(ventilatorData[1]);

                        addDataToChart(flowChart,flow);
                        addDataToChart(pressureChart,pressure);
                     }
                }
          });
    })
});