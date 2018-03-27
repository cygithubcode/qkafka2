\l kfk.q
// create consumer process within group 0
client:.kfk.Consumer[`metadata.broker.list`group.id`auto.commit.enable`enable.auto.offset.store`auto.offset.reset!`192.168.81.134:9092`0`false`false`smallest];
//client:.kfk.Consumer[`metadata.broker.list`group.id`auto.offset.reset!`192.168.81.134:9092`0`earliest];
data:();


// setup meaningful consumer callback(do nothing by default)
.kfk.consumecb:{[msg]
    msg[`data]:"c"$msg[`data];
    msg[`rcvtime]:.z.p;
    data,::enlist msg;
	show msg;
	};
	

//data:();
rtdata:();
rd:();



.kfk.consumecb1:{[msg]
    msg[`data]:msg[`data];
    msg[`rcvtime]:.z.p;
    data,::enlist msg;
	show .z.P; show ckck::msg[`data];
	if[0<count msg[`data]; rtdata,::enlist msg[`data]
	show -9!msg[`data];
	rd,::enlist -9!msg[`data]
	
		];
		//bbk
	}
		
	
	
	
	
// subscribe to the "kx_tick" topic with default partitioning
.kfk.Sub[client;`kx_tick;enlist .kfk.PARTITION_UA];

