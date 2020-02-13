module type Config = {
	type params;
	type msg;

	// State that is carried by the subscription while it is active
	type state;

	let subscriptionName: string;

	let getUniqueId: params => string;

	let start: (~params: params, ~dispatch: 'msg => unit) => state;
	let update: (~params: params, ~state: state, ~dispatch: 'msg => unit) => state;
	let dispose: (~params: params, ~state: state) => unit;
};

type config('params, 'msg, 'state) = (module Config with
	type msg = 'msg and
	type params = 'params and
	type state = 'state);

type t('msg) =
| Subscription({
	config: config('params, 'msg, 'state),
	params: 'params,
	state: option('state),
}): t('msg);

module Make = (ConfigInfo: Config) => {
	 type params = ConfigInfo.params;
	 type msg = ConfigInfo.msg;
	 
	 let create = (params) => {
		Subscription({
			config: (module ConfigInfo),
			params,
			state: None,
		});
	 };
};

type subscription('msg) = t('msg);

module Runner {
	type t('msg) = {
		subscriptions: list(t('msg)),
	};

	let empty = () => { subscriptions: [] };

	let run = (
		~dispatch,
		~sub,
		state
	) => {
		ignore(dispatch);
		ignore(sub);
		state;
	}
};
