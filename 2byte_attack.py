def main():
    # Length of the plaintext
    L = 256

    # First and last bytes (known)
    u_1 = u_L = [69]

    # Initialize the candidates with the known first byte
    Q = [(u_1, 0)]

    for r in range(L):
        Q_next = []
        for u_next in range(256):
            Q_with_ending = []
            for (P, lambda_P) in Q:
                ending = u_L if r == L-1 else P[-1]
                lambda_P_next = probability(lambda_P, r, P[r-1], u_next, N, p)
                Q_with_ending += [(P + [u_next], lambda_P_next)]
            Q_next += [max_2(Q_with_ending)]

        Q = Q_next


def read_N():
    # return N after reading it from a file
    pass


def probability(lambda_P, r, u_r, u_next, N, p):
    # That probability mass function dealio
    return lambda_P + sum(N[r][k_1 ^ u_r][k_2 ^ u_next] * log(p[r][k_1][k_2]) for k_2 in range(256) for k_1 in range(256))


def max_2(l):
    pass