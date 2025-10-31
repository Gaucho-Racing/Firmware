# Configuration Changes

## Clock Accuracy

A Nucleo-G474RE does not have an external crystal, but Big Bird does

Once Big Bird is received switch from HSI (High-Speed Internal) to HSE (High-Speed External)

See 3a18f2a7ae1b65bfdcec40b5623e33ac09b32374 where it was found that the Nucleo requires HSI and the swithc HSE -> HSI took place
