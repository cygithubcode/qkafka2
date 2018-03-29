#ifndef WIN32
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <string.h>
#include <librdkafka/rdkafka.h>
#include "socketpair.c"
#include "k.h"

typedef unsigned int UI;
#define KR -128
#define KNL (K) 0
#define KFK_OK RD_KAFKA_RESP_ERR_NO_ERROR
// create dictionary q dictionary from list of items (s1;v1;s2;v2;...)
K xd0(I n, ...) __attribute__((sentinel));
K xd0(I n, ...) {
  va_list a;
  S s;
  K x, y= ktn(KS, 0), z= ktn(0, 0);
  va_start(a, n);
  for(; s= va_arg(a, S), s && (x= va_arg(a, K));)
    js(&y, ss(s)), jk(&z, x);
  va_end(a);
  return xD(y, z);
}
//my code 
int64_t timeoffsetint=-1;
// my code 

#define xd(...) xd0(0, __VA_ARGS__, (S) 0)
static K clients, topics;
static I spair[2];
static K S0;
// check type
// letter as usual, + for table, ! for dict
static I checkType(S tc, ...) {
  va_list args;
  K x;
  static C lt[256]= " tvunzdmpscfejihg xb*BX GHIJEFCSPMDZNUVT";
  static C b[256];
  S tc0= tc;
  I match=0;
  lt[20 + 98]= '+';
  lt[20 + 99]= '!';
  va_start(args, tc);
  for(; *tc;) {
    match= 0;
    x= va_arg(args, K);
    if(!x) {
      strcpy(b, "incomplete type string ");
      break;
    };
    if('[' == *tc) {
      while(*tc && ']' != *tc) {
        match= match || lt[20 + xt] == *tc;
        ++tc;
      }
    } else
      match= lt[20 + xt] == *tc;
    if(!match) {
      strcat(strcpy(b, "type:expected "), tc0);
      break;
    };
    ++tc;
  }
  va_end(args);
  if(!match)
    krr(b);
  return match;
}

// use QS
rd_kafka_t *clientIndex(K x) {
  return (rd_kafka_t *) ((((UI) xi < clients->n) && kS(clients)[xi]) ?
                             kS(clients)[xi] :
                             (S) krr("unknown client"));
}
rd_kafka_topic_t *topicIndex(K x) {
  return (rd_kafka_topic_t *) ((((UI) xi < topics->n) && kS(topics)[xi]) ?
                                   kS(topics)[xi] :
                                   (S) krr("unknown topic"));
}
// print error if any and release K object.
// should return 0 to indicate mem free to kafka where needed in callback
static I printr0(K x) {
  if(!x)
    return 0;
  if(KR == xt)
    fprintf(stderr, "%s\n", x->s);
  r0(x);
  return 0;
}
static I statscb(rd_kafka_t *rk, S json, size_t json_len, V *opaque) {
  return printr0(k(0, (S) ".kfk.statcb", kpn(json, json_len), KNL));
} // should return 0 to indicate mem free to kafka
static V logcb(const rd_kafka_t *rk, int level, const char *fac,
               const char *buf) {
  printr0(k(0, (S) ".kfk.logcb", ki(level), kp((S) fac), kp((S) buf), KNL));
}
// client api
// x - config dict sym->sym
static K loadConf(rd_kafka_conf_t *conf, K x) {
  char b[512];
  J i;
  for(i= 0; i < xx->n; ++i) {
    if(RD_KAFKA_CONF_OK !=
       rd_kafka_conf_set(conf, kS(xx)[i], kS(xy)[i], b, sizeof(b))) {
      return krr((S) b);
    }
  }
  return knk(0);
}
static K loadTopConf(rd_kafka_topic_conf_t *conf, K x) {
  char b[512];
  J i;
  for(i= 0; i < xx->n; ++i) {
    if(RD_KAFKA_CONF_OK !=
       rd_kafka_topic_conf_set(conf, kS(xx)[i], kS(xy)[i], b, sizeof(b))) {
      return krr((S) b);
    }
  }
  return knk(0);
}

// int query_offsets_by_time (rd_kafka_topic_partition_list_t *offsets) {
        // rd_kafka_resp_err_t err;
// #if RD_KAFKA_VERSION >= 0x00090300
        // char errstr[512];

        // if (rd_kafka_conf_set(conf.rk_conf, "api.version.request", "true",
                              // errstr, sizeof(errstr)) != RD_KAFKA_CONF_OK)
                // KC_FATAL("Failed to enable api.version.request: %s", errstr);

        // if (!(conf.rk = rd_kafka_new(RD_KAFKA_PRODUCER, conf.rk_conf,
                                     // errstr, sizeof(errstr))))
                // KC_FATAL("Failed to create producer: %s", errstr);

        // err = rd_kafka_offsets_for_times(conf.rk, offsets, 10*1000);
// #else
        // err = RD_KAFKA_RESP_ERR__NOT_IMPLEMENTED;
// #endif
        // if (err)
                // KC_FATAL("offsets_for_times failed: %s", rd_kafka_err2str(err));

        // partition_list_print(offsets, NULL);

        // rd_kafka_destroy(conf.rk);

        // return 0;
// }


static void  my_rebalance_cb (rd_kafka_t *rk, rd_kafka_resp_err_t err,
                      rd_kafka_topic_partition_list_t *partitions, void *opaque) {
	if (timeoffsetint>=0){
		rd_kafka_topic_partition_t *part2;
		if ((part2 = rd_kafka_topic_partition_list_find(partitions, "kxfeed_trade", 0)))
		   part2->offset = timeoffsetint; // 1519266278118;
		
		rd_kafka_resp_err_t err2;   
		err2= rd_kafka_offsets_for_times(rk, partitions, 10*1000);
		
		if (err2) 
			printf("offsets_for_times failed: %s", rd_kafka_err2str(err2));
	

	
   if (err == RD_KAFKA_RESP_ERR__ASSIGN_PARTITIONS) {
       rd_kafka_topic_partition_t *part;
       if ((part = rd_kafka_topic_partition_list_find(partitions, "kxfeed_trade", 0)))
		   //printf(part->offset);
	       //part->offset = 260;
       rd_kafka_assign(rk, partitions);
   }  else {
       rd_kafka_assign(rk, NULL);
   }
   
   	};
   
}

// x:client type p - producer, c - consumer
// y:config dict sym->sym
K kfkClient(K x, K y) {
  rd_kafka_type_t type;
  rd_kafka_t *rk;
  rd_kafka_conf_t *conf;
  char b[512];
  if(!checkType("c!", x, y))
    return KNL;
  if('p' != xg && 'c' != xg)
    return krr("type: unknown client type");
  type= 'p' == xg ? RD_KAFKA_PRODUCER : RD_KAFKA_CONSUMER;
  if(!loadConf(conf= rd_kafka_conf_new(), y))
    return KNL;


  // rd_kafka_offsets_for_times(conf, offsets, 10*1000);

  //my code 
  rd_kafka_conf_set_rebalance_cb(conf, my_rebalance_cb);
 //my code 

  rd_kafka_conf_set_stats_cb(conf, statscb);
  rd_kafka_conf_set_log_cb(conf, logcb);
  if(!(rk= rd_kafka_new(type, conf, b, sizeof(b))))
    return krr(b);
  /* Redirect rd_kafka_poll() to consumer_poll() */
  if(type == RD_KAFKA_CONSUMER){
    rd_kafka_poll_set_consumer(rk);
    rd_kafka_queue_io_event_enable(rd_kafka_queue_get_consumer(rk),spair[1],"X",1);
  }else{
    rd_kafka_queue_io_event_enable(rd_kafka_queue_get_main(rk),spair[1],"X",1);
  }

  js(&clients, (S) rk);
  return ki(clients->n - 1);
}

K kfkClientDel(K cid) {
  rd_kafka_t *rk;
  if(!checkType("i", cid))
    return KNL;
  if(!(rk= clientIndex(cid)))
    return KNL;
  rd_kafka_consumer_close(rk);
  rd_kafka_destroy(rk);
  kS(clients)[cid->i]= (S) 0;
  return KNL;
}
K kfkClientName(K cid) {
  rd_kafka_t *rk;
  if(!checkType("i", cid))
    return KNL;
  if(!(rk= clientIndex(cid)))
    return KNL;
  return ks((S) rd_kafka_name(rk));
}
K kfkClientMemberId(K cid) {
  rd_kafka_t *rk;
  if(!checkType("i", cid))
    return KNL;
  if(!(rk= clientIndex(cid)))
    return KNL;
  return ks(rd_kafka_memberid(rk));
}

// topic api
K kfkTopic(K cid, K topic, K tconf) {
  rd_kafka_topic_t *rkt;
  rd_kafka_t *rk;
  rd_kafka_topic_conf_t *rd_topic_conf;
  if(!checkType("is!", cid, topic, tconf))
    return KNL;
  if(!(rk= clientIndex(cid)))
    return KNL;
  rd_topic_conf= rd_kafka_topic_conf_new();
  loadTopConf(rd_topic_conf, tconf);
  rkt= rd_kafka_topic_new(rk, topic->s, rd_topic_conf);
  js(&topics, (S) rkt);
  return ki(topics->n - 1);
}
K kfkTopicDel(K tid) {
  rd_kafka_topic_t *rkt;
  if(!checkType("i", tid))
    return KNL;
  if(!(rkt= topicIndex(tid)))
    return KNL;
  rd_kafka_topic_destroy(rkt);
  kS(topics)[tid->i]= (S) 0;
  return KNL;
}
K kfkTopicName(K tid) {
  rd_kafka_topic_t *rkt;
  if(!checkType("i", tid))
    return KNL;
  if(!(rkt= topicIndex(tid)))
    return KNL;
  return ks((S) rd_kafka_topic_name(rkt));
}

// metadata api
// rd_kafka_metadata_broker: `id`host`port!(id;host;port)
K decodeMetaBroker(rd_kafka_metadata_broker_t *x) {
  return xd("id", ki(x->id), "host", ks(x->host), "port", ki(x->port));
}
// rd_kafka_metadata_partition: `id`err`leader`replicas`isrs!(int;err;int;int
// list;int list)
K decodeMetaPart(rd_kafka_metadata_partition_t *p) {
  K x= ktn(KI, p->replica_cnt);
  J i;
  for(i= 0; i < xn; ++i)
    kI(x)[i]= p->replicas[i];
  K y= ktn(KI, p->isr_cnt);
  for(i= 0; i < y->n; ++i)
    kI(y)[i]= p->isrs[i];
  return xd("id", ki(p->id), "err", ks((S) rd_kafka_err2str(p->err)), "leader",
            ki(p->leader), "replicas", x, "isrs", y);
}

// rd_kafka_metadata_topic: `topic`partitions`err!(string;partition list;err)
K decodeMetaTopic(rd_kafka_metadata_topic_t *t) {
  K x= ktn(0, 0);
  J i;
  for(i= 0; i < t->partition_cnt; ++i)
    jk(&x, decodeMetaPart(&t->partitions[i]));
  return xd("topic", ks(t->topic), "err", ks((S) rd_kafka_err2str(t->err)),
            "partitions", x);
}

// rd_kafka_metadata: `brokers`topics`orig_broker_id`orig_broker_name!(broker
// list;topic list;int;string)
K decodeMeta(const rd_kafka_metadata_t *meta) {
  K x= ktn(0, 0);
  J i;
  for(i= 0; i < meta->broker_cnt; ++i)
    jk(&x, decodeMetaBroker(&meta->brokers[i]));
  K y= ktn(0, 0);
  for(i= 0; i < meta->topic_cnt; ++i)
    jk(&y, decodeMetaTopic(&meta->topics[i]));
  return xd("orig_broker_id", ki(meta->orig_broker_id), "orig_broker_name",
            ks(meta->orig_broker_name), "brokers", x, "topics", y);
}

// f[int] // ;topic_id;timeout]
K kfkMetadata(K cid) {
  const struct rd_kafka_metadata *meta;
  K r;
  rd_kafka_t *rk;
  if(!checkType("i", cid))
    return KNL;
  if(!(rk= clientIndex(cid)))
    return KNL;
  rd_kafka_resp_err_t err= rd_kafka_metadata(rk, 1, NULL, &meta, 5000);
  if(KFK_OK != err)
    return krr((S) rd_kafka_err2str(err));
  r= decodeMeta(meta);
  rd_kafka_metadata_destroy(meta);
  return r;
}

// producer api
K kfkPub(K tid, K partid, K data, K key) {
  rd_kafka_topic_t *rkt;
  if(!checkType("ii[CG][CG]", tid, partid, data, key))
    return KNL;
  if(!(rkt= topicIndex(tid)))
    return KNL;
  if(rd_kafka_produce(rkt, partid->i, RD_KAFKA_MSG_F_COPY, kG(data), data->n,
                      kG(key), key->n, NULL))
    return krr((S) rd_kafka_err2str(rd_kafka_last_error()));
  return KNL;
}

// consume api
K kfkSub(K cid, K topic, K partitions) {
  rd_kafka_resp_err_t err;
  rd_kafka_t *rk;
  J i;
  if(!checkType("isI", cid, topic, partitions))
    return KNL;
  if(!(rk= clientIndex(cid)))
    return KNL;
  rd_kafka_topic_partition_list_t *t_partition=
      rd_kafka_topic_partition_list_new(partitions->n);
  for(i= 0; i < partitions->n; ++i)
    rd_kafka_topic_partition_list_add(t_partition, topic->s, kI(partitions)[i]);
  if(KFK_OK != (err= rd_kafka_subscribe(rk, t_partition)))
    return krr((S) rd_kafka_err2str(err));
  return knk(0);
}
	
	
void kfkSetTimeoffsetj(K toffset){
	timeoffsetint=toffset->j;
	};
	
	
	
	
	
K kfkSub2(K cid, K topic, K partitions, K toffset) { 

	timeoffsetint=toffset->j;

  rd_kafka_resp_err_t err;
  rd_kafka_t *rk;
  J i;
  if(!checkType("isI", cid, topic, partitions))
    return KNL;
  if(!(rk= clientIndex(cid)))
    return KNL;

   // moved .. rd_kafka_conf_set_rebalance_cb(conf.rk_conf, rebalance_cb);
 
 
  rd_kafka_topic_partition_list_t *t_partition=
      rd_kafka_topic_partition_list_new(partitions->n);

  //rd_kafka_topic_partition_t *part;
  for(i= 0; i < partitions->n; ++i){
	  
    rd_kafka_topic_partition_list_add(t_partition, topic->s, kI(partitions)[i]); // ->offset=1519266278118 ; // (toffset->j);
	//part->offset = 260;
  };
  //dd rd_kafka_topic_partition_list_add(t_partition, topic->s, 1)->offset=5;
  // my test code.. 
   // rd_kafka_topic_partition_t *part;
       // if ((part = rd_kafka_topic_partition_list_find(partitions, "kxfeed_trade", 0)))
           // part->offset = 260;
	 // my test code.. ^  
	   
  rd_kafka_assign(rk, t_partition);

  
  
  if(KFK_OK != (err= rd_kafka_subscribe(rk, t_partition)))
    return krr((S) rd_kafka_err2str(err));

  // rd_kafka_topic_partition_list_set_offset(t_partition, topic->s, kI(partitions)[i], 5);

  return knk(0); 
}

K kfkSub2a(K cid, K topic, K partitions) {
  rd_kafka_resp_err_t err;
  rd_kafka_t *rk;
  J i;
  if(!checkType("isI", cid, topic, partitions))
    return KNL;
  if(!(rk= clientIndex(cid)))
    return KNL;

   // moved .. rd_kafka_conf_set_rebalance_cb(conf.rk_conf, rebalance_cb);
 
 
  rd_kafka_topic_partition_list_t *t_partition=
      rd_kafka_topic_partition_list_new(partitions->n);

 
  for(i= 0; i < partitions->n; ++i){
	 rd_kafka_topic_partition_t *part;  
    part=rd_kafka_topic_partition_list_add(t_partition, topic->s, kI(partitions)[i]); // ->offset=5;
	part->offset = 260;
  };
  //dd rd_kafka_topic_partition_list_add(t_partition, topic->s, 1)->offset=5;
  // my test code.. 
   // rd_kafka_topic_partition_t *part;
       // if ((part = rd_kafka_topic_partition_list_find(partitions, "kxfeed_trade", 0)))
           // part->offset = 260;
	 // my test code.. ^  
	   
  rd_kafka_assign(rk, t_partition);

  
  
  if(KFK_OK != (err= rd_kafka_subscribe(rk, t_partition)))
    return krr((S) rd_kafka_err2str(err));

  // rd_kafka_topic_partition_list_set_offset(t_partition, topic->s, kI(partitions)[i], 5);

  	// rd_kafka_topic_partition_t *part2;
	// if ((part2 = rd_kafka_topic_partition_list_find(t_partition, "kxfeed_trade", 0)))
	   // part2->offset = 1519266278118;
  
  
  return knk(0);
}


K kfkUnsub(K cid) {
  rd_kafka_t *rk;
  rd_kafka_resp_err_t err;
  if(!checkType("i", cid))
    return KNL;
  if(!(rk= clientIndex(cid)))
    return KNL;
  err= rd_kafka_unsubscribe(rk);
  if(KFK_OK != err)
    return krr((S) rd_kafka_err2str(err));
  return knk(0);
}

K decodeTopPar(rd_kafka_topic_partition_t *tp) {
  return xd("topic", ks((S) tp->topic), "partition", ki(tp->partition),
            "offset", kj(tp->offset), "metadata",
            kpn(tp->metadata, tp->metadata_size));
}

K kfkSubscription(K cid) {
  K r;
  J i;
  rd_kafka_topic_partition_list_t *t;
  rd_kafka_t *rk;
  rd_kafka_resp_err_t err;
  if(!checkType("i", cid))
    return KNL;
  if(!(rk= clientIndex(cid)))
    return KNL;
  err= rd_kafka_subscription(rk, &t);
  if(KFK_OK != err)
    return krr((S) rd_kafka_err2str(err));
  r= ktn(0, t->cnt);
  for(i= 0; i < r->n; ++i)
    kK(r)[i]= decodeTopPar(&t->elems[i]);
  rd_kafka_topic_partition_list_destroy(t);
  return r;
}
static J pu(J u){return 1000000LL*(u-10957LL*86400000LL);}
// `mtype`topic`partition`data`key`offset`opaque
K decodeMsg(rd_kafka_message_t *msg) {
  K x= ktn(KG, msg->len), y=ktn(KG, msg->key_len), z;
  J ts= rd_kafka_message_timestamp(msg, NULL);
  memmove(kG(x), msg->payload, msg->len);
  memmove(kG(y), msg->key, msg->key_len);
  z= ktj(-KP, ts > 0 ? pu(ts) : nj);
  return xd0(0, "mtype",
             msg->err ? ks((S) rd_kafka_err2name(msg->err)) : r1(S0), "topic",
             msg->rkt ? ks((S) rd_kafka_topic_name(msg->rkt)) : r1(S0),
             "partition", ki(msg->partition), "offset", kj(msg->offset),
             "msgtime", z, "data", x, "key", y, (S) 0);
}

K kfkPoll(K cid, K timeout, K maxcnt) {
  K r;
  J n= 0;
  rd_kafka_message_t *msg;
  rd_kafka_t *rk;
  rd_kafka_type_t rk_type;
  if(!checkType("ijj", cid, timeout, maxcnt))
    return KNL;
  if(!(rk= clientIndex(cid)))
    return KNL;
  rk_type= rd_kafka_type(rk);
  if(rk_type == RD_KAFKA_PRODUCER) {
    n= rd_kafka_poll(rk, timeout->j);
    return kj(n);
  }
  int maxmsgs= maxcnt && maxcnt->j ? maxcnt->j : wi;
  while((n < maxmsgs) && (msg= rd_kafka_consumer_poll(rk, timeout->j))) {
    r= decodeMsg(msg);
    printr0(k(0, ".kfk.consumecb", r, KNL));
    rd_kafka_message_destroy(msg);
    n++;
  }
  return kj(n);
}
// other
K kfkOutQLen(K cid) {
  rd_kafka_t *rk;
  if(!(rk= clientIndex(cid)))
    return KNL;
  return ki(rd_kafka_outq_len(rk));
}
K kfkVersion(K _) { return ki(rd_kafka_version()); }
K kfkExportErr(K _) {
  const struct rd_kafka_err_desc *errdescs;
  size_t n;
  J i;
  K x= ktn(0, 0), y= ktn(0, 0), z= ktn(0, 0);
  rd_kafka_get_err_descs(&errdescs, &n);
  for(i= 0; i < n; ++i)
    if(errdescs[i].code) {
      jk(&x, ki(errdescs[i].code));
      jk(&y, ks((S)(errdescs[i].name ? errdescs[i].name : "")));
      jk(&z, kp((S)(errdescs[i].desc ? errdescs[i].desc : "")));
    }
  return xT(xd("errid", x, "code", y, "desc", z));
}
// shared lib loading
K kfkCallback(I d) {
  char buf[1024];int i,consumed=0;
  /*MSG_DONTWAIT - set in sd1(-h,...) */
  while(0 < recv(d, buf, sizeof(buf), 0))
    ;
  for(i= 0; i < clients->n; i++) {
    K c= kfkPoll(ki(i), kj(0), kj(10000));
    consumed+= c->j;
    r0(c);
  }
  return KNL;
}

__attribute__((constructor)) V __attach(V) {
  if(dumb_socketpair(spair, 1) == -1){
    fprintf(stderr, "Init failed. socketpair: %s\n", strerror(errno));
    return;
  }
  clients= ktn(KS, 0);
  topics= ktn(KS, 0);
  S0= ks("");
  printr0(sd1(-spair[0], &kfkCallback));
}

__attribute__((destructor)) V __detach(V) {
  I sp,i;
  if(topics) {
    for(i= 0; i < topics->n; i++)
      kfkTopicDel(ki(i));
    r0(topics);
  }
  if(clients) {
    for(i= 0; i < clients->n; i++)
      kfkClientDel(ki(i));
    rd_kafka_wait_destroyed(1000); /* wait for cleanup*/
    r0(clients);
  }
  sp= spair[0];
  spair[0]= 0;
  sd0(sp);
  close(sp);
  sp= spair[1];
  spair[1]= 0;
  close(sp);
}
