type t('model, 'msg) = ('model, 'msg) => ('model, Effect.t('msg));

let ofReducer = (reducer, model, msg) => {
  (reducer(model, msg), Effect.none);
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
