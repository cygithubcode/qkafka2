\l kfk.q
// create consumer process within group 0
client:.kfk.Consumer[`metadata.broker.list`group.id!`192.168.81.134:9092`0];
data:();
// setup meaningful consumer callback(do nothing by default)
//.kfk.consumecb:{[msg]
//    msg[`data]:"c"$msg[`data];
//    msg[`rcvtime]:.z.p;
//    data,::enlist msg;}
//// subscribe to the "test" topic with default partitioning
//.kfk.Sub[client;`test;enlist .kfk.PARTITION_UA];

 ///// 
 
 
kfk_cfg:(!) . flip(
    (`metadata.broker.list;`192.168.81.134:9092);
    (`group.id;`0);
	(`auto.offset.reset;`smallest)
   / (`queue.buffering.max.ms;`1);
   / (`fetch.wait.max.ms;`10);
   / (`statistics.interval.ms;`10000)
    );
client:.kfk.Consumer[kfk_cfg];
data:();
rtdata:();
rd:();



.kfk.consumecb:{[msg]
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
	
//but we don't want to sub, we just want to get all the old data
.kfk.Sub[client;`kx_tick;enlist .kfk.PARTITION_UA];

client_meta:.kfk.Metadata[client];
show client_meta`topics;
