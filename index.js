var mosca = require('mosca')
var MicroGear = require('microgear')
var topics = {}
var deviceName = {}
// define config netpie
const APPID = 'SmartOfficeAt418B'
const KEY = 'OGtRNotdKadsXwp'
const SECRET = 'kYiE3YDsHK1QXf9d6SPajoHtT';
var microgear = MicroGear.create({
  key: KEY,
  secret: SECRET
});
microgear.on('connected', function () {
  console.log('Connected...');
  microgear.setAlias("broker");
});

var pubsubsettings = {
  type: 'redis',
  redis: require('redis'),
  db: 12,
  port: 6379,
  return_buffers: true, // to handle binary payloads
  host: "localhost"
};

var moscaSettings = {
  port: 1883		//mosca (mqtt) port
};

var server = new mosca.Server(moscaSettings);	//here we start mosca

server.on('ready', setup);	//on init it fires up setup()
server.on('clientConnected', function (client) { // when client connect
  console.log('client connected', client.id);
  deviceName[client.id] = [] // collect device's name
});
server.on('clientDisconnected', function (client) { // when client disconnect
  console.log('Client Disconnected:', client.id);
  deviceName[client.id].map(topic =>
    microgear.unsubscribe(topic) // unsubscribe netpie ,that device subscribed.
  )
  delete deviceName[client.id] // delete device'name out of list
});
// fired when the mqtt server is ready
function setup() {
  console.log('Mosca server is up and running')
  server.authenticate = authenticate;
  server.authorizeSubscribe = authorizeSubscribe;
}
// authenticate root or user
var authenticate = function (client, username, password, callback) {
  var authorized = (username === 'root' && password.toString() === '1q2w3e4r') ||
    (username === 'user' && password.toString() === '1234')
  if (authorized) client.user = username;
  callback(null, authorized);
}

var authorizeSubscribe = function (client, topic, callback) {
  topicAr = topic.split('/')
  let deviceNameSub = (topicAr[0] === 'gearLocal') ? topicAr[1] : topicAr[0]
  let authorized = true
  if (client.user === 'root') authorized = true   // root can subscribe all
  else {  // check authorize for normal user 
    if (client.id === deviceNameSub) { // check device's name .you want to subscribe
      let newTopic = '/gearname/' + deviceNameSub // extent gearname before
      microgear.subscribe(newTopic) // subscribe that topic for recive chat to 
      deviceName[client.id].push(newTopic) // push topic, that subscribed on netpie
      authorized = true
    }
    else if (topic === '#' || topic === '/#') authorized = false // check wildcard subscribe
    else if (topicAr[0] === 'gearLocal') authorized = false // user cannot subscribe other'chat
    else if (topicAr[0] === 'netpie') { // want to subscribe topic on netpie
      let topicSubNetpie = topicAr.slice(1).join('/')
      authorized = true
      microgear.subscribe('/' + topicSubNetpie) // subscribe that topic on netpie 
      deviceName[client.id].push('/' + topicSubNetpie)  // push topic, that subscribed on netpie
    }
    else if (!deviceName[deviceNameSub]) authorized = !deviceName[deviceNameSub]
    else authorized = true
  }
  callback(null, authorized);
}
// fired when a client disconnects

// publish inside
function pub(topic, payload) {
  var message = {
    topic: topic,
    payload: payload, // or a Buffer
    qos: 0, // 0, 1, or 2
    retain: false // or true
  };
  server.publish(message, function () {
    //console.log(topic,payload)
  });

}
// fired when a message is published
server.on('published', function (packet, client) {
  header = packet.topic.split('/')
  msg = packet.payload.toString()
  headerSub = header.slice(1).join('/')
  if (!topics[packet.topic]) topics[packet.topic] = 'in'
  console.log(topics[packet.topic], packet.topic, msg)
  if (topics[packet.topic] === 'in') {
    if (header[0] === 'gearLocal') { // check keyword chat
      //check Is device on inside 
      if (!deviceName[header[1]])
        // if deivce is outside then chat to netpie
        microgear.chat(header[1], msg);
    }
    else if (header[0] === 'global') { // check keyword publish to netpie
      // publish to netpie
      microgear.publish(`/${headerSub}`, msg, false);
    }
  }

});

//recrive message from netpie
microgear.on('message', function (topic, body) {
  let header = topic.split('/')
  header.shift()
  header.shift()
  let msg = body.toString()
  if (!topics[topic]) topics[topic] = 'out'
  console.log(topics[packet.topic], packet.topic, msg)
  if (topics[topic] === 'out') {
    if (header[0] === 'gearname') { //it is chat??
      if (deviceName[header[1]]) // is device on inside?
        pub('gearLocal/' + header[1], msg) // if device is inside ,msg will published
    }
    else pub('netpie/' + header.join('/'), msg) //into publish edge
  }

});
microgear.connect(APPID);
