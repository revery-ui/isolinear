type subscriber('msg) = {
  id: int,
  onValue: 'msg => unit,
  /* onComplete: complete, */
};

type t('msg) = {
  lastSubscriberId: ref(int),
  subscribers: ref(list(subscriber('msg))),
};

let ofDispatch = f => {
  let subscribers: ref(list(subscriber('msg))) = ref([]);

  let onValue = value => List.iter(sub => sub.onValue(value), subscribers^);

  f(onValue);

  {lastSubscriberId: ref(0), subscribers};
};

let create = () => {
  let dispatcher = ref(None);

  let stream = ofDispatch(dispatch => dispatcher := Some(dispatch));

  let dispatch = msg => Option.iter(dispatch => dispatch(msg), dispatcher^);

  (stream, dispatch);
};

let subscribe = (stream, onValue) => {
  incr(stream.lastSubscriberId);
  let id = stream.lastSubscriberId^;

  stream.subscribers := [{id, onValue}, ...stream.subscribers^];

  let unsubscribe = () =>
    stream.subscribers :=
      List.filter(sub => sub.id != id, stream.subscribers^);

  unsubscribe;
};

let connect = (dispatch: 'msg => unit, stream: t('msg)) =>
  subscribe(stream, dispatch);

let filterMap = (stream, f: 'a => option('b)) => {
  ofDispatch(send =>
    subscribe(stream, msg => f(msg) |> Option.iter(send))
    |> (ignore: (unit => unit) => unit)
  );
};
