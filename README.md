# lemonade

Lemonade is downstream of [bergamot](https://github.com/browsermt), and
targeted for linux, maybe other \*nix.

This is a loose compilation of multiple scripts, with no care for dependencies
when put together should work on my machine. Highly opinionated, there's limited
configurability in the ultimate command-line tool or services provided by this
repository.

For library, cross-platfrom, configurability look at the applications offered
by [bergamot-translator](https://github.com/browsermt/bergamot-translator). You
might also want to checkout
[translateLocally](https://github.com/XapaJIaMnu/translateLocally).

 - Only the best openly available models between two languages supported by
   bergamot-translator will be available under the hood.

 - There are only options to choose the source-language and target-language. No
   optional anything, you get everything in a Response that we know from
   translation. 

