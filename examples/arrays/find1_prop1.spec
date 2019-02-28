// The find example from the Invgen-repo where we want to prove that
// if we find a position, the element at that position is equal to v

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
	(=>
		(<= 0 alength)
		(or
			(= (r main_end) alength)
			(= (a (r main_end)) v)
		)
	)
)
