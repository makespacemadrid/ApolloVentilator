"use strict"; 

 $(function(){
    $(document).ready(function(){
  
        let pressureChartC  = 'pressureChartC';
        let valveChartC     = 'valveChartC';
        let flowChartC      = 'flowChartC';

        let pressureData          = [];
        let intakeInstantFlowData = [];
        let exitInstantFlowData   = [];
        let intakeFlowData        = [];
        let exitFlowData          = [];
        let intakeValveData       = [];
        let ExitValveData         = [];

        let chartRange = [[0,0],[0,0]];

        let trace1   = { x: [], y: [], mode: 'lines', name: 'pressure', line:{color :'#00d8d2'}, hoverinfo:'skip'};
        let trace2   = { x: [], y: [], mode: 'lines', name: 'intakeValve', line:{color:'#fed72d'}, hoverinfo:'skip' };
        let trace3   = { x: [], y: [], mode: 'lines', name: 'ExitValve', line:{color:'#ff766c'}, hoverinfo:'skip' };
        let trace4   = { x: [], y: [], mode: 'lines', name: 'intakeInstantFlow', line:{color:'#ff766c'}, hoverinfo:'skip' };

        let layout   = { xaxis: { tickformat: ':%S' }, yaxis: { autorange:false, range: [0,0],  scaleratio: .1}, margin: { l: 50, r: 0, b: 20, t: 5, pad: 0 }};
        let settings = { displayModeBar: false, responsive: true, displaylogo: false };

        let dataC1   = [ trace1 ];
        let dataC2   = [ trace2, trace3 ];
        let dataC3   = [ trace4 ];
        
        let socket   = io.connect('/');                     

        socket.on('serialPortData', function (data){

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

                    let now       = new Date();

                    chartRange[0] = [ Math.min(chartRange[0][0], pressure)   , Math.max(chartRange[0][1], pressure) ];
                    chartRange[1] = [ Math.min(chartRange[1][0], intakeValve), Math.max(chartRange[1][1], intakeValve) ];


                    trace1.x.push(now); trace1.y.push(pressure);
                    trace2.x.push(now); trace2.y.push(intakeValve);
                    trace3.x.push(now); trace3.y.push(ExitValve);

                    // trace4.x.push(now);trace4.y.push(intakeInstantFlow);
                        
                    if(trace1.x.length > 200){
                        trace1.x.shift();trace1.y.shift();
                        trace2.x.shift();trace2.y.shift();
                        trace3.x.shift();trace3.y.shift();
                    }
                }
            }
        });

        window.setInterval(function(){
            layout.yaxis.range = chartRange[0];
            Plotly.newPlot(pressureChartC, dataC1, layout, settings); 

            layout.yaxis.range = chartRange[1];
            Plotly.newPlot(valveChartC, dataC2, layout, settings); 
        }, 80);

    })
});