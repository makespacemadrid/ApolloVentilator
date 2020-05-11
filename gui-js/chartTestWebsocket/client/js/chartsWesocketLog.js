"use strict"; 

 $(function(){
    $(document).ready(function() {
  
        let pressureChartC  = document.getElementById('pressureChartC');
        let valveChartC     = document.getElementById('valveChartC');
        let flowChartC      = document.getElementById('flowChartC');

        let pressureData          = [];
        let intakeInstantFlowData = [];
        let exitInstantFlowData   = [];
        let intakeFlowData        = [];
        let exitFlowData          = [];
        let intakeValveData       = [];
        let ExitValveData         = [];


        let pressureChart = new TimeChart(pressureChartC, {
            baseTime: Date.now() - performance.now(),
            series: [{
                name: 'pressure',
                data: pressureData,
                color: '#1eb7ff'
            }],
            xRange: { min: 0, max: 5 * 1000 },
            realTime: true,
            zoom: {
                x: {
                    autoRange: false,
                    minDomainExtent: 150,
                },
                y: {
                    autoRange: false,
                    minDomainExtent: 1,
                }
            }
        });

        let valveChart = new TimeChart(valveChartC, {
            baseTime: Date.now() - performance.now(),
            series: [{
                    name: 'intakeValve',
                    data: intakeValveData,
                    color: '#28a745'
                },
                {
                    name: 'ExitValve',
                    data: ExitValveData,
                    color: '#d73a49'
                }
            ],
            xRange: { min: 0, max: 5 * 1000 },
            realTime: true,
            zoom: {
                x: {
                    autoRange: false,
                    minDomainExtent: 150,
                },
                y: {
                    autoRange: false,
                    minDomainExtent: 1,
                }
            }
        });

        let flowChart = new TimeChart(flowChartC, {
            baseTime: Date.now() - performance.now(),
            series: [{
                    name: 'intakeInstantFlow',
                    data: intakeInstantFlowData,
                    color: '#6f42c1',
                },
                {
                    name: 'exitInstantFlow',
                    data: exitInstantFlowData,
                    color: '#f66a0a',
                }
            ],
            xRange: { min: 0, max: 5 * 1000 },
            realTime: true,
            zoom: {
                x: {
                    autoRange: false,
                    minDomainExtent: 150,
                },
                y: {
                    autoRange: false,
                    minDomainExtent: 1,
                }
            }
        });

        let socket = io.connect('/');

        socket.on('serialPortData', function (data) {

            let rawCommand = data.data.split(':');
            let commandID  = rawCommand[0];

            if (commandID === 'DATA'){
                let ventilatorData = rawCommand[1].split(',');

                if (ventilatorData.length > 2){

                        //{pressure,intakeInstantFlow,exitInstantFlow,intakeFlow,exitFlow,intakeValve,ExitValve}
                        
                        let pressure           = parseFloat(ventilatorData[0]);
                        let intakeInstantFlow  = parseFloat(ventilatorData[1]);
                        let exitInstantFlow    = parseFloat(ventilatorData[2]);
                        let intakeFlow         = parseFloat(ventilatorData[3]);
                        let exitFlow           = parseFloat(ventilatorData[4]);
                        let intakeValve        = parseFloat(ventilatorData[5]);
                        let ExitValve          = parseFloat(ventilatorData[6]);


                        const    x = performance.now() - 20*1000;
                        const time = performance.now();


                        if((pressureData.length)>500){
                            //console.log(pressureData.length)
                        }
      
                        pressureData.push({ x, y:pressure });
                        intakeInstantFlowData.push({ x, y:intakeInstantFlow });
                        exitInstantFlowData.push({ x, y:exitInstantFlow });


                        intakeFlowData.push({ x, y:intakeFlow });
                        exitFlowData.push({ x, y:exitFlow });
                        intakeValveData.push({ x, y:intakeValve });
                        ExitValveData.push({ x, y:ExitValveData });


                        if(pressureData.length > 1000){
                            //pressureData.shift();
                           // pressureChart.series.data = pressureData;
                        }

                

                      //  flowChart.update();

                     }
                }
          });

        window.setInterval(function(){
            pressureChart.update();
            valveChart.update();
        }, 100);
    });
});