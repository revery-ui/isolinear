type t('msg, 'model) = ('model, 'msg) => ('model, Effect.t('msg));

let ofReducer: (('model, 'msg) => 'model) => t('msg, 'model);

let combine: list(t('msg, 'model)) => t('msg, 'model);
