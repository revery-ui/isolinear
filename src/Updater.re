type t('actions, 'state) =
  ('state, 'actions) => ('state, Effect.t('actions));

type reducer('actions, 'state) = ('state, 'actions) => 'state;

let ofReducer = (v: reducer('actions, 'state), s, a) => {
  (v(s, a), Effect.none);
};
