type dispatcher('msg) = 'msg => unit;
type unsubscribe = unit => unit;

module Effect = Effect;
module Store = Store;
module Stream = Stream;
module Updater = Updater;

module Sub = Sub;

module Internal = {
  module Sub = Sub;
  module SubscriptionRunner = SubscriptionRunner;
};
