let create =
    (~initialState: 'state, ~updater: Updater.t('actions, 'state), ()) => {
  let state = ref(initialState);

  let sender = ref(None);

  let stream = Stream.create((send) => {
      sender := Some(send);
  })

  let dispatch = action => {
    let currentState = state^;
    let (newState, effect) = updater(currentState, action);
    state := newState;

    switch (sender^) {
    | None => ()
    | Some(s) => s((newState, action));
    };

    (newState, effect);
  };

  (dispatch, stream);
};
