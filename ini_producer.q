\l kfk.q
kfk_cfg:(!) . flip(
  (`metadata.broker.list;`192.168.81.134:9092);
  (`statistics.interval.ms;`10000);
  (`queue.buffering.max.ms;`1);
  (`fetch.wait.max.ms;`10)
  );
producer:.kfk.Producer[kfk_cfg]
test_topic:.kfk.Topic[producer;`kx_tick;()!()]

.z.ts:{show string x; .kfk.Pub[test_topic;.kfk.PARTITION_UA;string x;"from q session "]}
//.z.ts:{show string x; .kfk.Pub[test_topic;.kfk.PARTITION_UA;-8!x;"from q session "]}

show "Publishing on topic:",string .kfk.TopicName test_topic;
//.kfk.Pub[test_topic;.kfk.PARTITION_UA;string .z.p;"start from q session "];
//.kfk.Pub[test_topic;.kfk.PARTITION_UA;string .z.p;"start from q session "];


show "Published 1 message";
producer_meta:.kfk.Metadata[producer];
show producer_meta`topics;
show "Set timer with \\t 1000 to publish message every second";

