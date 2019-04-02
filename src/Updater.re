type t('actions, 'state) =
  ('state, 'actions) => ('state, Effect.t('actions));

type reducer('actions, 'state) = ('state, 'actions) => 'state;

let ofReducer = (v: reducer('actions, 'state), s, a) => {
  (v(s, a), Effect.none);
};

let combine = (updaters, state, action) => {
  let (newState, effects) =
    List.fold_left(
      (prev, curr) => {
        let (prevState, prevEffects) = prev;
        let (newState, effect) = curr(prevState, action);
        (newState, [effect, ...prevEffects]);
      },
      (state, [Effect.none]),
      updaters,
    );

  let effects = effects |> List.filter(e => e !== Effect.none) |> List.rev;

  (newState, Effect.batch(effects));
};
