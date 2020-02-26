type t('model, 'msg) = ('model, 'msg) => ('model, Effect.t('msg));

let ofReducer: (('model, 'msg) => 'model) => t('model, 'msg);
let combine: list(t('model, 'msg)) => t('model, 'msg);
