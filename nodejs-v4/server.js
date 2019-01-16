var express = require('express');
var app = express();
var request = require('request');
app.use(express.static('public'));
 
 
var Symbol = "";
var sm = Array();
app.get('/123/', function (req, res) {
    console.log("req:"+Object.keys(req)) ;
    console.log("reqquery:"+Object.keys(req.query));
    console.log("reqquery:"+Object.keys(req.params)) ;
   // 输出 JSON 格式
   var response = {
       "Symbol":req.query.symbol
   };
   Symbol = req.query.symbol;
   var url = new Array();
    url[0]=  'https://www.alphavantage.co/query?function=TIME_SERIES_DAILY&symbol='+Symbol+'&outputsize=full&apikey=466BB044M86GP7JM';
    url[1]= "https://www.alphavantage.co/query?function=STOCH&symbol="+Symbol+"&interval=daily&slowkmatype=1&slowdmatype=1&time_period=10&apikey=466BB044M86GP7JM";
    url[2] = "https://www.alphavantage.co/query?function=BBANDS&symbol="+Symbol+"&interval=daily&time_period=5&series_type=close&nbdevup=3&nbdevdn=3&apikey=466BB044M86GP7JM";
    url[3] = "https://www.alphavantage.co/query?function=SMA&symbol="+Symbol+"&interval=daily&time_period=10&series_type=close&apikey=466BB044M86GP7JM";
    url[4]= "https://www.alphavantage.co/query?function=EMA&symbol="+Symbol+"&interval=daily&time_period=10&series_type=close&apikey=466BB044M86GP7JM";
    url[5] = "https://www.alphavantage.co/query?function=RSI&symbol="+Symbol+"&interval=daily&time_period=10&series_type=close&apikey=466BB044M86GP7JM";
    url[6] = "https://www.alphavantage.co/query?function=CCI&symbol="+Symbol+"&interval=daily&time_period=10&series_type=close&apikey=466BB044M86GP7JM";
    url[7] = "https://www.alphavantage.co/query?function=MACD&symbol="+Symbol+"&interval=daily&time_period=10&series_type=close&apikey=466BB044M86GP7JM";
    url[8] = "https://www.alphavantage.co/query?function=ADX&symbol="+Symbol+"&interval=daily&time_period=10&series_type=close&apikey=466BB044M86GP7JM";
    url[9] = "https://seekingalpha.com/api/sa/combined/"+Symbol+".xml"
    url[10]= 'https://www.alphavantage.co/query?function=TIME_SERIES_INTRADAY&symbol='+Symbol+'&interval=1min&apikey=466BB044M86GP7JM';
  var Data = new Object();
  	var num = 0;
   request.get({
    url: url[0],
    json: true,
    headers: {'User-Agent': 'request'}
    }, (err, resq, data) => {
        if (err) {
         console.log('Error:', err);
        } 
        else if (resq.statusCode !== 200) {
         console.log('Status:', resq.statusCode);
        } else {
      // data is already parsed as JSON:
      //console.log(data);
         Data['price']=data;
         //console.log(Data[i]);
      
          }
          num++;
          console.log(0);
          console.log(num);
          if(num>=11){
          	res.send(JSON.stringify(Data));
          }
        
    });
       
   request.get({
    url: url[1],
    json: true,
    headers: {'User-Agent': 'request'}
    }, (err, resq, data) => {
        if (err) {
         console.log('Error:', err);
        } 
        else if (resq.statusCode !== 200) {
         console.log('Status:', resq.statusCode);
        } else {
      // data is already parsed as JSON:
      //console.log(data);
         Data['STOCH']=data;
         //console.log(Data[i]);
      
          }
          num++;
          console.log(1);
          console.log(num);
          if(num>=11){
          res.send(JSON.stringify(Data));
          }
         
    });
       
          request.get({
    url: url[2],
    json: true,
    headers: {'User-Agent': 'request'}
    }, (err, resq, data) => {
        if (err) {
         console.log('Error:', err);
        } 
        else if (resq.statusCode !== 200) {
         console.log('Status:', resq.statusCode);
        } else {
      // data is already parsed as JSON:
      //console.log(data);
         Data['BBANDS']=data;
         //console.log(Data[i]);
      
          }
          console.log(2);
         num++;
         console.log(num);
          if(num>=11){
          	res.send(JSON.stringify(Data));
          }
          
    });
       
    request.get({
    url: url[3],
    json: true,
    headers: {'User-Agent': 'request'}
    }, (err, resq, data) => {
        if (err) {
         console.log('Error:', err);
        } 
        else if (resq.statusCode !== 200) {
         console.log('Status:', resq.statusCode);
        } else {
      // data is already parsed as JSON:
      //console.log(data);
         Data['SMA']=data;
         //console.log(Data);
      
          }
          console.log(3);
         num++;
         console.log(num);
          if(num>=11){
          	res.send(JSON.stringify(Data));
          }

    });
       
    request.get({
    url: url[4],
    json: true,
    headers: {'User-Agent': 'request'}
    }, (err, resq, data) => {
        if (err) {
         console.log('Error:', err);
        } 
        else if (resq.statusCode !== 200) {
         console.log('Status:', resq.statusCode);
        } else {
      // data is already parsed as JSON:
      //console.log(data);
         Data['EMA']=data;
         //console.log(Data[i]);
      
          }
          console.log(4);
          num++;
          console.log(num);
          if(num>=11){
          	res.send(JSON.stringify(Data));
          }
    });
       
    request.get({
    url: url[5],
    json: true,
    headers: {'User-Agent': 'request'}
    }, (err, resq, data) => {
        if (err) {
         console.log('Error:', err);
        } 
        else if (resq.statusCode !== 200) {
         console.log('Status:', resq.statusCode);
        } else {
      // data is already parsed as JSON:
      //console.log(data);
         Data['RSI']=data;
         //console.log(Data[i]);
      
          }
          console.log(5);
          num++;
          console.log(num);
          if(num>=11){
          	res.send(JSON.stringify(Data));
          }
    });

    request.get({
    url: url[6],
    json: true,
    headers: {'User-Agent': 'request'}
    }, (err, resq, data) => {
        if (err) {
         console.log('Error:', err);
        } 
        else if (resq.statusCode !== 200) {
         console.log('Status:', resq.statusCode);
        } else {
      // data is already parsed as JSON:
      //console.log(data);
         Data['CCI']=data;
         //console.log(Data[i]);
      
          }
          console.log(6);
          num++;
          console.log(num);
          if(num>=11){
          	res.send(JSON.stringify(Data));
          }
    });
  
    request.get({
    url: url[7],
    json: true,
    headers: {'User-Agent': 'request'}
    }, (err, resq, data) => {
        if (err) {
         console.log('Error:', err);
        } 
        else if (resq.statusCode !== 200) {
         console.log('Status:', resq.statusCode);
        } else {
      // data is already parsed as JSON:
      //console.log(data);
         Data['MACD']=data;
         //console.log(Data[i]);
      
          }
          console.log(7);
          num++;
          console.log(num);
          if(num>=11){
          	res.send(JSON.stringify(Data));
          }
    });

    request.get({
    url: url[8],
    json: true,
    headers: {'User-Agent': 'request'}
    }, (err, resq, data) => {
        if (err) {
         console.log('Error:', err);
        } 
        else if (resq.statusCode !== 200) {
         console.log('Status:', resq.statusCode);
        } else {
      // data is already parsed as JSON:
      //console.log(data);
         Data['ADX']=data;
         //console.log(Data[i]);
      
          }
          console.log(8);
          num++;
          console.log(num);
          if(num>=11){
          	res.send(JSON.stringify(Data));
          }
    });
    request.get({
    url: url[9],
    json: true,
    headers: {'User-Agent': 'request'}
    }, (err, resq, data) => {
        if (err) {
         console.log('Error:', err);
        } 
        else if (resq.statusCode !== 200) {
         console.log('Status:', resq.statusCode);
        } else {
      // data is already parsed as JSON:
      //console.log(data);
         var parseString = require('xml2js').parseString;
         var xml = data;
         parseString(xml, function (err, result) {
           //console.dir(result);
           Data['news']=result;
         });
         
         //console.log(Data[i]);
      
          }
          console.log(9);
          num++;
          console.log(num);
          if(num>=11){
          	res.send(JSON.stringify(Data));
          }
    });
    request.get({
    url: url[10],
    json: true,
    headers: {'User-Agent': 'request'}
    }, (err, resq, data) => {
        if (err) {
         console.log('Error:', err);
        } 
        else if (resq.statusCode !== 200) {
         console.log('Status:', resq.statusCode);
        } else {
      // data is already parsed as JSON:
      //console.log(data);
         Data['INTRADAY']=data;
         //console.log(Data[i]);
      
          }
          console.log(10);
          num++;
          console.log(num);
          if(num>=11){
          	res.send(JSON.stringify(Data));
          }
    });

});

app.get('/456/', function (req, res) {
    console.log("req:"+Object.keys(req)) ;
    console.log("reqquery:"+Object.keys(req.query));
    console.log("reqquery:"+Object.keys(req.params)) ;
   // 输出 JSON 格式
   var response = {
       "Symbol":req.query.symbol
   };

   Symbol = req.query.symbol;
   var url_ = 'https://www.alphavantage.co/query?function=TIME_SERIES_INTRADAY&symbol='+Symbol+'&interval=1min&apikey=466BB044M86GP7JM';
 
   request.get({
    url: url_,
    json: true,
    headers: {'User-Agent': 'request'}
    }, (err, resq, data) => {
        if (err) {
         console.log('Error:', err);
        } 
        else if (resq.statusCode !== 200) {
         console.log('Status:', resq.statusCode);
        } else {
      // data is already parsed as JSON:
      //console.log(data);
         var Data=data;
         //console.log(Data[i]);
      
          }
          console.log(Symbol);
        
          	res.send(JSON.stringify(Data));
          
        
    });
});

app.get('/789/*', function (req, res) {
    console.log("req:"+Object.keys(req)) ;
    console.log("reqquery:"+Object.keys(JSON.stringify(req.query)));
    console.log("reqquery:"+Object.keys(req.params)) ;
   // 输出 JSON 格式
   var response = {
       "Symbol":req.query.symbol
   };
   
   Symbol = req.query.symbol;
   var url2= 'http://dev.markitondemand.com/MODApis/Api/v2/Lookup/json?input='+Symbol;
 
   request.get({
    url: url2,
    json: true,
    headers: {'User-Agent': 'request'}
    }, (err, resq, data) => {
        if (err) {
         console.log('Error:', err);
        } 
        else if (resq.statusCode !== 200) {
         console.log('Status:', resq.statusCode);
        } else {
      // data is already parsed as JSON:
      //console.log(data);
         var Data=data;
         //console.log(Data[i]);
      
          }
          console.log(Symbol);
        
          	res.send(JSON.stringify(Data));
          
        
    });
});

var server = app.listen(8081, function () {
 
  var host = server.address().address
  var port = server.address().port
 
  console.log("应用实例，访问地址为 http://%s:%s", host, port)
 
})