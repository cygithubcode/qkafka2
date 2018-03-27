/ calculate ohlc vwap volume

args:.Q.def[(enlist `tp)!(enlist `:5001)].Q.opt .z.x;
h:hopen`$":",string args[`tp]

\l ini_consumer.q

t:`trade;s:`;	

/t3sec:();

t3sec:([]time:`timestamp$();sym:`symbol$();price:`float$();size:`int$();stop:`boolean$();cond:`char$();ex:`char$());

//date:`date$();

//secbar:([`s#time:`second$();`g#sym:`symbol$()];open:`float$();high:`float$();low:`float$();close:`float$();vwap:`float$();volume:`long$());

secbar:([`s#time:`timestamp$();`g#sym:`symbol$()];open:`float$();high:`float$();low:`float$();close:`float$();vwap:`float$();volume:`long$());

 upd:{[t;x]
    if[(0<count x) & t~`trade;
    if[not 98=type x;x:flip x]; //handle the differrence between real feed and logfile.
    t3sec,::x;

    //Can use rolling calculaiton for vwap, add up mv=size*price ,and add up size, then update vwap=mv%total size, and with calculation on lists instead of table could make it faster. Here we do a simple way for 3 second data calculation, then upsert to final table.
    `secbar upsert select open: first price, high: max price,low: min price,close:last price,vwap:size wavg price, volume:sum size by "p"$1e9 xbar time,sym from t3sec;
    //show t3sec;
    
    
    mt:(exec max time from t3sec)-00:00:03;
    delete from `t3sec where time < mt;
    
    //here or put in .z.ts to clean memory, limit table memory usage to 800MB for test sample
    //if[(838860800) < -22!secbar; delete from `secbar where i<0.1*count i; .Q.gc[]; ];
    
    ];    
    }

.z.ts:{if[(838860800) < -22!secbar; delete from `secbar where i<0.1*count i;];.Q.gc[];};

\t 2000

.u.rep:{(.[;();:;].) x;show y;if[null first y;:()];

	// -11!y;

	system "cd ",1_-10_string first reverse y};
.u.rep . h "(.u.sub[`trade;`];`.u `i`L)";
.u.end:{@[`.;tables`.;0#];}
