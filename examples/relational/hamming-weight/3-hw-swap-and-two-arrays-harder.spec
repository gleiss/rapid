// Variation 3: Example 2 is extended by adding the Hamming weight of an another array b (identical in both traces).
// This example corresponds to a simpler version of the Hamming-weight example where the splitting of the loop into three parts is done manually.

(set-traces 2)

func main()
{
	const Int alength;
	const Int[] a;
	const Int blength;
	const Int[] b;
	const Int x;
	const Int y;

	Int i = 0;
	Int hammingWeight = 0;

	while (i < alength)
	{
		if (a[i] != 0)
		{
			hammingWeight = hammingWeight + 1;
		}
		else
		{
			skip;
		}
		i = i + 1;
	}

	if (x != 0)
	{
		hammingWeight = hammingWeight + 1;
	}
	else
	{
		skip;
	}

	if (y != 0) 
	{
		hammingWeight = hammingWeight + 1;
	}
	else
	{
		skip;
	}

	i = 0;
	while (i < blength)
	{
		if (b[i] != 0)
		{
			hammingWeight = hammingWeight + 1;
		}
		else
		{
			skip;
		}
		i = i + 1;
	}
}

(axiom
	(<= 0 (alength t1))
)
(axiom
	(<= 0 (alength t2))
)
(axiom
	(<= 0 (blength t1))
)
(axiom
	(<= 0 (blength t2))
)

(conjecture
	(=>
		(and
			(= (alength t1) (alength t2))
			(= (blength t1) (blength t2))
			(forall ((posA Int))
				(=>
					(and
						(<= 0 posA)
						(< posA (alength t1))
					)
					(= (a posA t1) (a posA t2))
				)
			)
			(forall ((posB Int))
				(=>
					(and
						(<= 0 posB)
						(< posB (blength t1))
					)
					(= (b posB t1) (b posB t2))
				)
			)
			(= (x t1) (y t2))
			(= (y t1) (x t2))
		)
		(= (hammingWeight main_end t1) (hammingWeight main_end t2))
	)
)
