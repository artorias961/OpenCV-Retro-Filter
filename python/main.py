import cv2

def bayer_matrix_8x8():
    # Classic 8x8 Bayer matrix (values 0..63)
    return [
        [ 0, 48, 12, 60,  3, 51, 15, 63],
        [32, 16, 44, 28, 35, 19, 47, 31],
        [ 8, 56,  4, 52, 11, 59,  7, 55],
        [40, 24, 36, 20, 43, 27, 39, 23],
        [ 2, 50, 14, 62,  1, 49, 13, 61],
        [34, 18, 46, 30, 33, 17, 45, 29],
        [10, 58,  6, 54,  9, 57,  5, 53],
        [42, 26, 38, 22, 41, 25, 37, 21],
    ]

def apply_ordered_dither(bgr, strength=18):
    """
    Ordered dithering on each channel.
    strength: how strong the dither offset is (typ. 8..28)
    """
    h, w = bgr.shape[:2]
    bm = bayer_matrix_8x8()

    # Work in int16 to avoid overflow/underflow while adding offsets
    out = bgr.astype('int16')

    for y in range(h):
        row = bm[y % 8]
        for x in range(w):
            t = row[x % 8]  # 0..63
            # Map threshold to roughly [-strength/2, +strength/2]
            offset = int((t - 31.5) / 63.0 * strength)
            out[y, x, 0] += offset
            out[y, x, 1] += offset
            out[y, x, 2] += offset

    return out.clip(0, 255).astype('uint8')

def kmeans_quantize(bgr, k=16, attempts=3):
    """
    Reduce colors to k using OpenCV kmeans.
    """
    Z = bgr.reshape((-1, 3)).astype('float32')

    criteria = (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 30, 1.0)
    _, labels, centers = cv2.kmeans(
        Z, k, None, criteria, attempts, cv2.KMEANS_PP_CENTERS
    )

    centers = centers.astype('uint8')
    quant = centers[labels.flatten()].reshape(bgr.shape)
    return quant

def gba_retro_filter(
    bgr,
    target_width=240,       # GBA-ish internal width
    palette_colors=16,      # try 8, 16, 24, 32
    dither_strength=18,     # 0 disables dithering
    add_edge_hint=True      # helps the "sprite-ish" look
):
    h, w = bgr.shape[:2]

    # 1) Mild contrast boost (handheld-ish punch)
    # Convert to YCrCb and adjust Y
    ycc = cv2.cvtColor(bgr, cv2.COLOR_BGR2YCrCb)
    y, cr, cb = cv2.split(ycc)
    y = cv2.convertScaleAbs(y, alpha=1.10, beta=4)  # tweak alpha/beta as needed
    ycc = cv2.merge([y, cr, cb])
    bgr = cv2.cvtColor(ycc, cv2.COLOR_YCrCb2BGR)

    # 2) Downscale to low internal resolution (pixelation base)
    scale = target_width / float(w)
    target_height = max(1, int(h * scale))
    small = cv2.resize(bgr, (target_width, target_height), interpolation=cv2.INTER_AREA)

    # 3) Optional edge hint (subtle outline effect)
    if add_edge_hint:
        gray = cv2.cvtColor(small, cv2.COLOR_BGR2GRAY)
        edges = cv2.Canny(gray, 60, 140)
        edges = cv2.dilate(edges, None, iterations=1)
        edges_bgr = cv2.cvtColor(edges, cv2.COLOR_GRAY2BGR)
        # darken pixels where edges are present
        small = cv2.subtract(small, (edges_bgr // 2))

    # 4) Ordered dithering before quantization (helps reduce banding)
    if dither_strength > 0:
        small = apply_ordered_dither(small, strength=dither_strength)

    # 5) Palette reduction (GBA-ish limited palette vibe)
    small_q = kmeans_quantize(small, k=palette_colors)

    # 6) Nearest-neighbor upscale back to original size for crisp pixels
    out = cv2.resize(small_q, (w, h), interpolation=cv2.INTER_NEAREST)

    # 7) Optional sharpen (keep it light; pixel art can get harsh)
    kernel = cv2.getStructuringElement(cv2.MORPH_RECT, (3, 3))
    blurred = cv2.GaussianBlur(out, (3, 3), 0)
    out = cv2.addWeighted(out, 1.15, blurred, -0.15, 0)

    return out

def main():
    in_path = "test.jpg"
    out_path = "output_gba.png"

    img = cv2.imread(in_path, cv2.IMREAD_COLOR)
    if img is None:
        raise SystemExit(f"Could not read image: {in_path}")

    filtered = gba_retro_filter(
        img,
        target_width=240,
        palette_colors=16,
        dither_strength=18,
        add_edge_hint=True
    )

    cv2.imwrite(out_path, filtered)
    print("Wrote:", out_path)

if __name__ == "__main__":
    main()
