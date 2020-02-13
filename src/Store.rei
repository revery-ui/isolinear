type t('msg, 'model);

type unsubscribe = unit => unit;

let create: (
	~updater: Updater.t('msg, 'model),
	'model,
) => t('msg, 'model);

let getModel: t('msg, 'model) => 'model;

let dispatch: (t('msg, 'model), 'msg) => unit;

let onModelChanged: (t('msg, 'model), ('model) => unit) => unsubscribe;
let onPendingEffect: (t('msg, 'model), unit => unit) => unsubscribe;

let hasPendingEffects: (t('msg, 'model)) => bool;
let runPendingEffects: (t('msg, 'model)) => unit;
