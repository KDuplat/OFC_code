#https://github.com/wanglab-georgetown/fractal/blob/main/fractal/fractal.py

import numpy as np
from scipy.interpolate import interp1d, splprep, splev
from scipy import stats

#%% Utils
def longest_consecutive_sequence(nums):
    """
    Find the longest consecutive sequence in a sorted list of numbers.

    Parameters:
        nums (list): A list of sorted numbers.

    Returns:
        list: Longest consecutive sequence.
    """
    nums = list(nums)
    if not nums:
        return []

    longest_sequence = []
    current_sequence = [nums[0]]

    for i in range(1, len(nums)):
        if nums[i] == nums[i - 1] + 1:  # Consecutive number
            current_sequence.append(nums[i])
        else:  # Sequence breaks
            if len(current_sequence) > len(longest_sequence):
                longest_sequence = current_sequence
            current_sequence = [nums[i]]  # Start a new sequence

    # Final check after the loop
    if len(current_sequence) > len(longest_sequence):
        longest_sequence = current_sequence

    return longest_sequence


def find_ranges_pct(cs, ss, pct=0.01):
    """
    Find ranges using a threshold-based method.

    Parameters:
        cs (ndarray): Array of box counts.
        ss (ndarray): Array of scales.
        pct (float): Percentage threshold.

    Returns:
        tuple: Filtered counts, scales, and their indices.
    """
    max_box = np.max(cs)
    th0 = pct
    th1 = 1.00 - pct

    idx0 = (
        np.where(cs <= th0 * max_box)[0][-1] + 1 if np.any(cs <= th0 * max_box) else 0
    )
    idx1 = np.where(cs >= th1 * max_box)[0][0]

    xt = cs[idx0 : idx1 + 1]
    st = ss[idx0 : idx1 + 1]
    return xt, st, idx0, idx1


def find_ranges_ls(cs, ss, use_log=True):
    """
    Find ranges using a least-squares regression method.

    Parameters:
        cs (ndarray): Array of box counts.
        ss (ndarray): Array of scales.
        use_log (bool): Whether to use log-transformed values. Defaults to True.

    Returns:
        tuple: Filtered counts, scales, and their indices.
    """
    max_box = np.max(cs)
    idx0 = 0
    idx1 = np.where(cs == max_box)[0][0]

    xt = cs[idx0 : idx1 + 1]
    st = ss[idx0 : idx1 + 1]
    idxs = np.arange(idx0, idx1 + 1)

    if use_log:
        zt = np.log10(xt)
    else:
        zt = xt

    slope, intercept, _, _, _ = stats.linregress(-np.log10(st), zt)

    yt = (
        np.power(10, slope * (-np.log10(st)) + intercept)
        if use_log
        else slope * (-np.log10(st)) + intercept
    )
    residuals = xt - yt

    valid_indices = np.where(residuals > 0)[0]
    if len(valid_indices) == 0:
        return xt, st, idx0, idx1

    sequence = longest_consecutive_sequence(valid_indices)
    idx0, idx1 = sequence[0], sequence[-1]

    return xt[idx0 : idx1 + 1], st[idx0 : idx1 + 1], idxs[idx0], idxs[idx1]


def odr_line(B, x):
    """
    Define a line for orthogonal distance regression.

    Parameters:
        B (array-like): Coefficients [intercept, slope].
        x (ndarray): Input x-values.

    Returns:
        ndarray: Computed y-values.
    """
    return B[1] * x + B[0]


def perform_odr(x, y):
    """
    Perform orthogonal distance regression (ODR) for a line.

    Parameters:
        x (ndarray): Input x-values.
        y (ndarray): Input y-values.

    Returns:
        ODR output: Regression results.
    """
    model = odr.Model(odr_line)
    data = odr.Data(x, y)
    odr_instance = odr.ODR(data, model, beta0=[0.0, 0.0])
    return odr_instance.run()


def get_reflex(cs_new, return_cost=False):
    """
    Detect reflex point based on least-squares regression.

    Parameters:
        cs_new (ndarray): Log-transformed counts and scales.
        return_cost (bool): Whether to return regression costs.

    Returns:
        dict: Reflex point index and optionally costs.
    """
    x = cs_new[:, 0]
    y = cs_new[:, 1]

    costs = []
    indices = range(2, len(x) - 2)

    for i in indices:
        segments = [(x[:i], y[:i]), (x[i:], y[i:])]
        total_cost = 0

        for segment_x, segment_y in segments:
            regression = perform_odr(segment_x, segment_y)
            total_cost += regression.sum_square

        costs.append(total_cost)

    min_idx = np.argmin(costs)
    reflex_idx = indices[min_idx]

    result = {"min_idx": reflex_idx}
    if return_cost:
        result["cost"] = costs

    return result


def plot_grid(xt, idx, s, use_lim=True, figsize=(9, 8)):
    m = int(1 / s) + 1

    fig = plt.figure(figsize=figsize)
    plt.plot(xt[:, 0], xt[:, 1], linewidth=1, c="r")
    for i in range(m):
        plt.plot([i * s, i * s], [0, 1], linewidth=0.5, c="b")
        plt.plot([0, 1], [i * s, i * s], linewidth=0.5, c="b")

    plt.scatter(idx[:, 0] * s + 0.5 * s, idx[:, 1] * s + 0.5 * s, marker=".")
    if use_lim:
        plt.xlim([0, 1])
        plt.ylim([0, 1])

    plt.title("{}x{} boxes".format(m, m), fontdict={"fontsize": 20})

    return fig


def gen_circle(n=5000):
    v = 2 * np.pi / n
    t = np.array(range(n))
    x = np.sin(v * t) + 1
    y = np.cos(v * t) + 1
    eps = 1e-10

    df = pd.DataFrame([x, y]).T
    df.columns = ["x", "y"]
    md = max(df.x.max() - df.x.min(), df.y.max() - df.y.min())
    df["nx"] = (df.x - df.x.min()) / md * (1 - eps) + 0.5 * eps
    df["ny"] = (df.y - df.y.min()) / md * (1 - eps) + 0.5 * eps

    xs = df[["nx", "ny"]].values
    return xs


def gen_line(xs0, n=5000, normalization=True):
    xs = []
    for t in np.array(xs0).T:
        xs.append(np.linspace(t[0], t[1], n))
    eps = 1e-10

    df = pd.DataFrame(xs).T
    df.columns = ["x", "y"]
    md = max(df.x.max() - df.x.min(), df.y.max() - df.y.min())
    df["nx"] = (df.x - df.x.min()) / md * (1 - eps) + 0.5 * eps
    df["ny"] = (df.y - df.y.min()) / md * (1 - eps) + 0.5 * eps

    if normalization is True:
        xs = df[["nx", "ny"]].values
    else:
        xs = df[["x", "y"]].values
    return xs


# https://github.com/TheAlgorithms/Python/blob/master/fractals/koch_snowflake.py
# ===================================
# Koch Snowflake Generator
# ===================================


def rotate(vector, angle_deg):
    """Rotate a vector by an angle."""
    theta = np.radians(angle_deg)
    rotation_matrix = np.array(
        [[np.cos(theta), -np.sin(theta)], [np.sin(theta), np.cos(theta)]]
    )
    return rotation_matrix @ vector


def iteration_step(vectors):
    """Perform a single Koch snowflake iteration."""
    new_vectors = []
    for i, start in enumerate(vectors[:-1]):
        end = vectors[i + 1]
        diff = (end - start) / 3
        new_vectors += [
            start,
            start + diff,
            start + diff + rotate(diff, 60),
            start + 2 * diff,
        ]
    new_vectors.append(vectors[-1])
    return new_vectors


def iterate(vectors, steps):
    """Iterate Koch snowflake generation."""
    for _ in range(steps):
        vectors = iteration_step(vectors)
    return vectors


def gen_koch(steps=4, shape="snowflake"):
    """Generate a Koch snowflake or curve."""
    initial_vectors = (
        [
            np.array([0, 0]),
            np.array([0.5, np.sqrt(3) / 2]),
            np.array([1, 0]),
            np.array([0, 0]),
        ]
        if shape == "snowflake"
        else [np.array([0, 0]), np.array([1, 0])]
    )

    vectors = iterate(initial_vectors, steps)
    df = pd.DataFrame(vectors, columns=["x", "y"])
    md = max(df.x.max() - df.x.min(), df.y.max() - df.y.min())
    eps = 1e-10
    df["nx"] = (df.x - df.x.min()) / md * (1 - eps) + 0.5 * eps
    df["ny"] = (df.y - df.y.min()) / md * (1 - eps) + 0.5 * eps
    return df[["nx", "ny"]].values



#%% Fractal functions:
def _get_box(points, scale):
    """
    Compute unique grid boxes for points scaled by `scale`.

    Parameters:
        points (ndarray): Array of points (n, 2).
        scale (float): Scale factor.

    Returns:
        list: Unique grid boxes.
    """
    scaled_boxes = [(int(pt[0] / scale), int(pt[1] / scale)) for pt in points]
    return list(set(scaled_boxes))


def _get_box_oversample(points, scale, oversample_rate=10000):
    """
    Compute oversampled unique grid boxes along the path.

    Parameters:
        points (ndarray): Array of points (n, 2).
        scale (float): Scale factor.
        oversample_rate (int): Number of points for interpolation.

    Returns:
        list: Unique oversampled grid boxes.
    """
    result = []
    for i in range(len(points) - 1):
        start = points[i, :]
        segment = points[i : i + 2, :] - start
        length = np.linalg.norm(segment[-1, :])

        if length == 0:
            continue

        tck, u = splprep(segment.T, k=1)
        u_new = np.linspace(u.min(), u.max(), oversample_rate)
        interpolated_points = np.array(splev(u_new, tck)).T + start

        x_scaled = (interpolated_points[:, 0] / scale).astype(int)
        y_scaled = (interpolated_points[:, 1] / scale).astype(int)

        result.extend(zip(x_scaled, y_scaled))
    return list(set(result))


def _get_box_exact(points, scale):
    """
    Compute exact grid boxes intersected by the path.

    Parameters:
        points (ndarray): Array of points (n, 2).
        scale (float): Scale factor.

    Returns:
        list: Unique exact grid boxes.
    """
    result = []
    for i in range(len(points) - 1):
        segment = points[i : i + 2]
        boxes = [tuple(s) for s in segment]

        # Traverse x direction
        x_sorted = np.sort(segment[:, 0])
        x_indices = np.arange(
            np.ceil(x_sorted[0] / scale), np.floor(x_sorted[1] / scale) + 1
        )
        if len(x_indices):
            y_interp = interp1d(segment[:, 0], segment[:, 1], fill_value="extrapolate")(
                x_indices * scale
            )
            boxes.extend(zip(x_indices * scale, y_interp))

        # Traverse y direction
        y_sorted = np.sort(segment[:, 1])
        y_indices = np.arange(
            np.ceil(y_sorted[0] / scale), np.floor(y_sorted[1] / scale) + 1
        )
        if len(y_indices):
            x_interp = interp1d(segment[:, 1], segment[:, 0], fill_value="extrapolate")(
                y_indices * scale
            )
            boxes.extend(zip(x_interp, y_indices * scale))

        boxes = sorted(set(boxes), key=lambda x: (x[0], x[1]))

        # Compute midpoints
        for j in range(len(boxes) - 1):
            mid_x = (boxes[j][0] + boxes[j + 1][0]) * 0.5 / scale
            mid_y = (boxes[j][1] + boxes[j + 1][1]) * 0.5 / scale
            result.append((int(mid_x), int(mid_y)))

    return list(set(result))


def _get_generalized(points, scale):
    """
    Compute generalized grid boxes and their weights.

    Parameters:
        points (ndarray): Array of points (n, 2).
        scale (float): Scale factor.

    Returns:
        dict: Grid boxes and their weights.
    """
    result = {}
    for i in range(len(points) - 1):
        segment = points[i : i + 2]
        boxes = [tuple(s) for s in segment]

        # Traverse x direction
        x_sorted = np.sort(segment[:, 0])
        x_indices = np.arange(
            np.ceil(x_sorted[0] / scale), np.floor(x_sorted[1] / scale) + 1
        )
        if len(x_indices):
            y_interp = interp1d(segment[:, 0], segment[:, 1], fill_value="extrapolate")(
                x_indices * scale
            )
            boxes.extend(zip(x_indices * scale, y_interp))

        # Traverse y direction
        y_sorted = np.sort(segment[:, 1])
        y_indices = np.arange(
            np.ceil(y_sorted[0] / scale), np.floor(y_sorted[1] / scale) + 1
        )
        if len(y_indices):
            x_interp = interp1d(segment[:, 1], segment[:, 0], fill_value="extrapolate")(
                y_indices * scale
            )
            boxes.extend(zip(x_interp, y_indices * scale))

        boxes = sorted(set(boxes), key=lambda x: (x[0], x[1]))

        # Compute weights
        for j in range(len(boxes) - 1):
            mid_x = (boxes[j][0] + boxes[j + 1][0]) * 0.5 / scale
            mid_y = (boxes[j][1] + boxes[j + 1][1]) * 0.5 / scale
            weight = np.linalg.norm(np.array(boxes[j]) - np.array(boxes[j + 1]))

            key = (int(mid_x), int(mid_y))
            result[key] = result.get(key, 0) + weight

    return result


def box_counting(
    points, scales, method="original", oversample_rate=2, return_boxes=False
):
    """
    Compute fractal dimension using the box counting method.

    Parameters:
        points (ndarray): Array of points (n, 2).
        scales (list): List of scales.
        method (str): Method to compute boxes ("original", "oversample", "exact").
        oversample_rate (int): Oversampling rate for "oversample" method.
        return_boxes (bool): Whether to return boxes at each scale.

    Returns:
        dict: Results with fractal dimension and fit statistics.
    """
    results = []
    for scale in scales:
        if method == "original":
            boxes = _get_box(points, scale)
        elif method == "oversample":
            boxes = _get_box_oversample(points, scale, oversample_rate)
        elif method == "exact":
            boxes = _get_box_exact(points, scale)
        else:
            raise ValueError(f"Unsupported method: {method}")

        results.append([scale, (len(boxes))])

    results = np.array(results)
    
    mask = results[:, 1] > 1
    f_results = results[mask]
    log_scales = -np.log(f_results[:-1, 0])
    log_boxes = np.log(f_results[:-1, 1])

    slope, _, r_value, p_value, _ = stats.linregress(log_scales[:5], log_boxes[:5])
    fd = slope

    result = {
        "fd": fd,
        "r_squared": r_value**2,
        "p_value": p_value,
    }
    if return_boxes:
        result["boxes"] = results

    return result


def box_counting_generalized(points, scales, q=2, return_boxes=False):
    """
    Compute generalized fractal dimension using exact box counting method.

    Parameters:
        points (ndarray): Array of points (n, 2).
        scales (list): List of scales.
        q (float): Order of the generalized fractal dimension.
        return_boxes (bool): Whether to return boxes at each scale.

    Returns:
        dict: Results with fractal dimension and fit statistics.
    """
    results = []
    for scale in scales:
        grid_weights = _get_generalized(points, scale)
        weights = np.array(list(grid_weights.values()))
        weights = weights / np.sum(weights)

        if q == 1:
            result = np.sum(weights * np.log(weights))
        elif q == 0:
            result = -np.log(np.sum(weights > 0))
        else:
            result = np.log(np.sum(np.power(weights, q))) / (q - 1)

        results.append([scale, result])

    results = np.array(results)

    slope, _, r_value, p_value, _ = stats.linregress(
        np.log(results[:, 0]), results[:, 1]
    )
    fd = slope

    output = {
        "fd": fd,
        "r_squared": r_value**2,
        "p_value": p_value,
    }
    if return_boxes:
        output["boxes"] = results

    return output


def temporal_sampling(
    points, min_step=1, max_step=2, q=1, start_index=0, return_boxes=False
):
    """
    Compute fractal dimension using the temporal sampling method.

    Parameters:
        points (ndarray): Array of points (n, 2).
        min_step (int): Minimum step size.
        max_step (int): Maximum step size.
        q (float): Order of the generalized fractal dimension.
        start_index (int): Starting index for sampling.
        return_boxes (bool): Whether to return computed points.

    Returns:
        dict: Results with fractal dimension and fit statistics.
    """
    results = []
    step_sizes = range(min_step, max_step + 1)

    for step in step_sizes:
        indices = range(start_index, len(points), step)
        sampled_points = points[indices]
        distances = np.linalg.norm(np.diff(sampled_points, axis=0), axis=1)
        avg_distance = np.mean(np.power(distances, q))
        results.append(avg_distance)

    results = np.array([step_sizes, results]).T

    slope, _, r_value, p_value, _ = stats.linregress(
        np.log(results[:, 0]), np.log(results[:, 1]) / q
    )
    fd = 1 + (1 - slope) * min(2, 1 / slope)

    output = {
        "fd": fd,
        "r_squared": r_value**2,
        "p_value": p_value,
    }
    if return_boxes:
        output["boxes"] = results

    return output


def corr_sum(points, scales, min_gap=1, return_boxes=False):
    """
    Compute the correlation sum for fractal dimension estimation.

    Parameters:
        points (ndarray): Array of points (n, 2).
        scales (list): List of scales.
        min_gap (int): Minimum separation of points.
        return_boxes (bool): Whether to return computed points.

    Returns:
        dict: Results with fractal dimension and fit statistics.
    """
    dx = np.subtract.outer(points[:, 0], points[:, 0])
    dy = np.subtract.outer(points[:, 1], points[:, 1])
    distances = np.sqrt(dx**2 + dy**2)

    for i in range(min_gap):
        np.fill_diagonal(distances[i:], np.nan)
        np.fill_diagonal(distances[:, i:], np.nan)

    results = []
    N = distances.shape[0]

    for scale in scales:
        count = np.sum(distances < scale) / ((N - min_gap) * (N + 1 - min_gap))
        results.append([scale, count])

    results = np.array(results)

    slope, _, r_value, p_value, _ = stats.linregress(
        np.log(results[:, 0]), np.log(results[:, 1])
    )
    fd = slope

    output = {
        "fd": fd,
        "r_squared": r_value**2,
        "p_value": p_value,
    }
    if return_boxes:
        output["boxes"] = results

    return output


def corr_sum_takens(points, min_gap=1, scale=None):
    """
    Compute the correlation sum using Takens' method.

    Parameters:
        points (ndarray): Array of points (n, 2).
        min_gap (int): Minimum separation of points.
        scale (float): Threshold scale for distances.

    Returns:
        float: Fractal dimension using Takens' method.
    """
    dx = np.subtract.outer(points[:, 0], points[:, 0])
    dy = np.subtract.outer(points[:, 1], points[:, 1])
    distances = np.sqrt(dx**2 + dy**2)

    for i in range(min_gap):
        np.fill_diagonal(distances[i:], np.nan)
        np.fill_diagonal(distances[:, i:], np.nan)

    if scale is None:
        # scale = np.std(points, axis=0).mean() / 4
        scale = np.sqrt(np.mean(np.var(points, axis=0))) / 4

    valid_distances = distances[(distances < scale) & (distances > 0)]
    count = len(valid_distances)
    fd = -(count - 1) / np.sum(np.log(valid_distances / scale))

    return fd


def find_elbow_scale(
    xs,
    init_scales=None,
    method="threshold",
    pct_th=0.01,
    return_boxes=False,
    init_scale_config=None,
):
    """
    Compute the elbow scale.

    Parameters:
        xs (ndarray): Array of points (n, 2).
        init_scales (list or None): Initial scales for computation. Defaults to None.
        method (str): Method to find elbow ("threshold" or "regression").
            Defaults to "threshold".
        pct_th (float): Threshold percentage for the "threshold" method.
            Defaults to 0.01.
        return_boxes (bool): Whether to return computed boxes. Defaults to False.
        init_scale_config (dict or None): Configuration for scale initialization.
            Keys can include:
            - "ref_scale": Reference scale for initialization. Defaults to mean distance.
            - "step_size": Step size for logarithmic scaling.
            - "num_scales": Total number of scales.
            - "num_scales_right": Number of scales to the right of reference scale.

    Returns:
        dict: A dictionary containing the results with indices, scales, and
            the minimum index.
    """
    if init_scale_config is None:
        init_scale_config = {}

    # Default scale initialization if not provided
    if init_scales is None:
        eps = 1e-20
        mean_distance = np.mean(np.linalg.norm(xs[:-1, :] - xs[1:, :], axis=1)) + eps
        ref_scale = init_scale_config.get("ref_scale", mean_distance)
        step_size = init_scale_config.get("step_size", 0.025)
        num_scales = init_scale_config.get("num_scales", 400)
        num_scales_right = init_scale_config.get("num_scales_right", 200)
        num_scales_left = num_scales - num_scales_right

        init_scales = ref_scale * np.power(
            10,
            step_size * np.linspace(num_scales_left, -num_scales_right, num_scales + 1),
        )

    # Perform box counting
    results = box_counting(xs, init_scales, method="original", return_boxes=True)
    boxes = results["boxes"]
    counts = boxes[:, 1]

    # Select method for range computation
    if method == "threshold":
        xt, st, idx0, idx1 = find_ranges_pct(counts, init_scales, pct=pct_th)
    elif method == "regression":
        xt, st, idx0, idx1 = find_ranges_ls(counts, init_scales)
    else:
        raise ValueError(f"Unknown method: {method}")

    rr = np.array([st, xt]).T
    # Compute log-transformed values for reflex detection
    log_rr = np.log10(rr)
    log_rr[:, 0] = -log_rr[:, 0]

    # Find reflex point
    reflex_result = get_reflex(log_rr)
    reflex_idx = reflex_result["min_idx"]

    # Prepare results
    result = {
        "idx0": idx0,
        "idx1": idx1,
        "min_idx": idx0 + reflex_idx,
        "scale": st[reflex_idx],
    }

    if return_boxes:
        result["boxes"] = rr

    return result