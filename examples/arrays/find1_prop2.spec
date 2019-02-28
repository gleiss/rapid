// The find example from the Invgen-repo where we want to prove that
// forall positions pos between 0 and r, A[pos] is different from v

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

(assert-not
	(forall ((pos Int))
		(=>
			(and
				(<= 0 pos)
				(< pos (r main_end))
			)
			(not (= (a pos) v))
		)
	)
)
