// The find example from the Invgen-repo where we want to prove that
// if we didn't find any position, then at all positions pos, A[pos] is different from v

func main()
{
	const Int[] a;
	const Int alength;
	const Int v;

	Int i = 0;
	Int r = alength;

	while (i < alength && r == alength)
	{
		if (a[i] == v)
		{
			r = i;
		}
		else
		{
			skip;
		}
		i = i + 1;
	}
}

(conjecture
	(forall ((pos Int))
		(=>
			(and
				(= (r main_end) alength)
				(<= 0 pos)
				(< pos alength)
			)
			(not (= (a pos) v))
		)
	)
)
