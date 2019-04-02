
type sendFunc('a) = 'a => unit;
/* type complete = unit => unit; */

type streamFunc('a) = (sendFunc('a)) => unit;

type subscriber('a) = {
    id: int,
    onValue: sendFunc('a),
    /* onComplete: complete, */
};

type t('a) = {
   lastSubscriberId: ref(int),
   subscribers: ref(list(subscriber('a))),
};

let ofDispatch = (f: streamFunc('a)) => {

    let subscribers: ref(list(subscriber('a))) = ref([]);

    let onValue = (v: 'a) => {
        List.iter((s) => s.onValue(v), subscribers^);
    }

    f(onValue);

    {
        lastSubscriberId: ref(0),
        subscribers
    }
};

let create = () => {
    let _dispatchFunction = ref(None);

    let stream = ofDispatch((dispatch) => {
        _dispatchFunction := Some(dispatch);
    });

    let dispatch = (v) => {
        switch(_dispatchFunction^) {
        | Some(f) => f(v)
        | None => ();
        }
    };

    (stream, dispatch)
};

let subscribe = (v: t('a), f: sendFunc('a)) => {
   let newId = v.lastSubscriberId^ + 1;
   v.lastSubscriberId := newId;

   let subscriber = {
    id: newId,
    onValue: f,
   };

   v.subscribers := [subscriber, ...v.subscribers^];
};

let map = (v: t('a), f: ('a) => option('b)) => {
   ofDispatch((send) => {
       subscribe(v, (x) => {
            switch (f(x)) {
            | None => ()
            | Some(v) => send(v);
            }
       });
   });
};

let connect = (dispatch: 'action => unit, stream: t('action)) => {
   subscribe(stream, (v) => dispatch(v)); 
};
