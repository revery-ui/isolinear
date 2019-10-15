type sendFunc('a) = 'a => unit;
/* type complete = unit => unit; */

type streamFunc('a) = sendFunc('a) => unit;

type unsubscribeFunc = unit => unit;

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
    List.iter(s => s.onValue(v), subscribers^);
  };

  f(onValue);

  {lastSubscriberId: ref(0), subscribers};
};

let create = () => {
  let _dispatchFunction = ref(None);

  let stream = ofDispatch(dispatch => _dispatchFunction := Some(dispatch));

  let dispatch = v => {
    switch (_dispatchFunction^) {
    | Some(f) => f(v)
    | None => ()
    };
  };

  (stream, dispatch);
};

let subscribe = (v: t('a), f: sendFunc('a)) => {
  let id = v.lastSubscriberId^ + 1;
  v.lastSubscriberId := id;

  let subscriber = {id, onValue: f};
  v.subscribers := [subscriber, ...v.subscribers^];

  let unsubscribe = () =>
    v.subscribers := List.filter(sub => sub.id != id, v.subscribers^);

  unsubscribe;
};

// TODO: This should be called `filterMap`
let map = (v: t('a), f: 'a => option('b)) => {
  ofDispatch(send =>
    subscribe(v, x =>
      switch (f(x)) {
      | None => ()
      | Some(v) => send(v)
      }
    )
    |> (ignore: unsubscribeFunc => unit)
  );
};

let connect = (dispatch: 'action => unit, stream: t('action)) => {
  subscribe(stream, v => dispatch(v));
};
