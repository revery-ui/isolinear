type dispatcher('msg) = 'msg => unit;
type unsubscribe = unit => unit;

module Stream = Stream;
module Effect = Effect;
module Updater = Updater;
module Sub = Sub;
module Store = Store;

module Internal = {
  module SubscriptionRunner = SubscriptionRunner;
};
