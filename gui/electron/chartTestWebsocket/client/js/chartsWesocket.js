"use strict"; 

 $(function(){
    $(document).ready(function() {
  
        let pressureChartC  = 'pressureChartC';
        let flowChartC      = 'flowChartC';

        var trace1 = { x: [], y: [], mode: 'lines', name: 'pressure', line:{color :'#1EB7FF'}, hoverinfo:'skip'};
        var trace2 = { x: [], y: [], mode: 'lines', name: 'flow', line:{color:'#1EB7FF'}, hoverinfo:'skip' };

        var layout   = { xaxis: { tickformat: ':%S' }, margin: { l: 50, r: 0, b: 20, t: 0, pad: 0 }};
        var settings = { displayModeBar: false, responsive: true, displaylogo: false };

        var dataC1     = [ trace1 ];
        var dataC2     = [ trace2 ];

        window.setInterval(function(){
            Plotly.newPlot(pressureChartC, dataC1, layout, settings); 
            Plotly.newPlot(flowChartC, dataC2, layout, settings); 
        }, 80);


        let socket = io.connect('http://localhost:8081');                    

        socket.on('serialPortData', function (data) {

            let rawCommand = data.data.split(':');
            let commandID  = rawCommand[0];

            if (commandID === 'DATA'){
                let ventilatorData = rawCommand[1].split(',');

                if (ventilatorData.length > 2){

                        //{pressure,intakeInstantFlow,exitInstantFlow,intakeFlow,exitFlow,intakeValve,ExitValve}
                        
                        let pressure           = parseFloat(ventilatorData[0]);
                        let intakeInstantFlow  = parseFloat(ventilatorData[1]);

                        var now = new Date();

                        trace1.x.push(now);trace1.y.push(pressure);
                        trace2.x.push(now);trace2.y.push(intakeInstantFlow);
                        
                        if(trace1.x.length > 200) {
                            trace1.x.shift();trace1.y.shift();
                            trace2.x.shift();trace2.y.shift();
                         //   trace4.x.shift();trace4.y.shift();
                        }

                     }
                }
          });
    })
});