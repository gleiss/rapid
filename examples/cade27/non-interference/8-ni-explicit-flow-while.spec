
(set-traces 2)

func main()
{
    Int hi;
    Int lo;
    Int dec;

    while (dec != 0)
    {
        hi = lo + 1;
        lo = hi + 1;
        dec = dec - 1;
    }
}

// Prove: if lo was the same at the beginning it is the same at the end (independently from the value of hi)
(conjecture
    (=>
        (and
            (forall ((tr Trace))
                (> (dec (l10 zero) tr) 0)
            )
            (= (dec (l10 zero) t1) (dec (l10 zero) t2) )
            (= (lo  (l10 zero) t1) (lo  (l10 zero) t2) )
        )
        (= (lo main_end t1) (lo main_end t2) )
    )
)
