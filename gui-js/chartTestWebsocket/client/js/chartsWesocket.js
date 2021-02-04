"use strict"; 

 $(function(){
    $(document).ready(function(){
  
        let pressureChartC  = 'pressureChartC';
        let flowChartC      = 'flowChartC';

        let chartRange = [[0,0],[0,0]];

        let trace1   = { x: [], y: [], mode: 'lines', name: 'pressure', line:{color :'#00d8d2'}, hoverinfo:'skip'};
        let trace2   = { x: [], y: [], mode: 'lines', name: 'flow', line:{color:'#fed72d'}, hoverinfo:'skip' };

        let layout   = { xaxis: { tickformat: ':%S' }, yaxis: { autorange:false, range: [0,0],  scaleratio: .1}, margin: { l: 50, r: 0, b: 20, t: 5, pad: 0 }};
        let settings = { displayModeBar: false, responsive: true, displaylogo: false };

        let dataC1   = [ trace1 ];
        let dataC2   = [ trace2 ];

        let socket   = io.connect('/');                     

        const eventToVentilator   = 'eventToVentilator';
        const eventFromVentilator = 'eventFromVentilator';

        socket.on(eventFromVentilator, function (payload){

            try{

                let jsonData     = JSON.parse(payload.data);
                let commandType  = jsonData.type;

                if (commandType === 'mData'){

                    let now      = new Date(); 
                    let pressure = jsonData.pressure;
                    let flow     = jsonData.iFlowInstant;

                    chartRange[0] = [ Math.min(chartRange[0][0], pressure), Math.max(chartRange[0][1], pressure) ];
                    chartRange[1] = [ Math.min(chartRange[1][0], flow), Math.max(chartRange[1][1], flow) ];

                    trace1.x.push(now); trace1.y.push(pressure);
                    trace2.x.push(now); trace2.y.push(flow);
           
                    if(trace1.x.length > 200){
                        trace1.x.shift();trace1.y.shift();
                        trace2.x.shift();trace2.y.shift();
                    }
                }
                else if (commandType ==='ventilatorConfig'){               
                    $("#rpm").val(jsonData.rpm);
                    $("#pMax").val(jsonData.pMax);
                    $("#pPeak").val(jsonData.pPeak);
                    $("#pPeep").val(jsonData.pPeep);
                    $("#vTidal").val(jsonData.vTidal);
                    $("#ieRatio").val(jsonData.ieRatio);
                    $("#iRiseTime").val(jsonData.iRiseTime);
                }
                else if (commandType === 'vStatus'){                  
                    $("#debugInfo").val(JSON.stringify(jsonData));
                }

            }catch(e){
                //console.log(e)
            }
        });

        window.setInterval(function(){
            layout.yaxis.range = chartRange[0];
            Plotly.newPlot(pressureChartC, dataC1, layout, settings); 

            layout.yaxis.range = chartRange[1];
            Plotly.newPlot(flowChartC, dataC2, layout, settings); 
        }, 80);

        $('.inputConfig').click(function(){
            this.select();
        });

        $("#startB").click(function() {
            socket.emit(eventToVentilator,{type:'command',command:'start'});
        });

        $("#pauseB").click(function() {
            socket.emit(eventToVentilator,{type:'command',command:'pause'});
        });

        $("#stopB").click(function() {
            socket.emit(eventToVentilator,{type:'command',command:'stop'});
        });

        $("#testB").click(function() {            
            socket.emit(eventToVentilator,{type:'command',command:'test'});
        });

        $("#calibrateB").click(function() {
            socket.emit(eventToVentilator,{type:'command',command:'calibrate'});
        });

        $("#getConfigB").click(function() {            
            socket.emit(eventToVentilator,{type:'command',command:'getConfig'});
        });

        $("#setConfigB").click(function() {                        
            let ventilatorConfig = { type:'ventilatorConfig' };

            $('.inputConfig').each(function(element, index, set) {
                ventilatorConfig[this.id] = this.value;
            });

            socket.emit(eventToVentilator, ventilatorConfig);
        });

    })
});