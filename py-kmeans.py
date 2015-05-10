import numpy as np

def l2_distance(x1, x2):
	return np.linalg.norm(x1-x2)

def find_nearest_centroid(point, centroids, distance=l2_distance):
	d_min = distance(point, centroids[0])
	idx = 0
	for i in range(1, len(centroids)):
		d = distance(point, centroids[i])
		if d < d_min:
			d_min = d
			idx = i
	return idx, d_min

def kmeans(data, n_clusters=10, init='random', tol=0.0001, max_iter=300, best_of=10):

	def _kmeans(data, n_clusters, init, tol, max_iter):

		if init == 'random':
			rand = range(data.shape[0])
			np.random.shuffle(rand)
			centroids = data[rand[:n_clusters]]
		else:
			raise NameError('Unknown initialization method')
		membership    = np.zeros(data.shape[0])
		new_centers   = np.zeros((n_clusters, data.shape[1]))
		count_centers = np.zeros(n_clusters)

		N     = data.shape[0]
		delta = float(N)
		iters = 0
		inertia = 0.0

		while delta / N > tol and iters < max_iter:
			delta = 0.0
			inertia = 0.0
			for i in xrange(N):
				center, distance = find_nearest_centroid(data[i], centroids)
				inertia += distance
				if membership[i] != center:
					delta += 1.0
					membership[i] = center
				new_centers[center] += data[i]
				count_centers[center] += 1.0
			for j in xrange(n_clusters):
				if count_centers[j] == 0:
					centroids[j] = data[np.random.random_integers(data.shape[0]-1)]
					delta += tol * N + 1.0
				else:
					centroids[j] = new_centers[j] / count_centers[j]
			new_centers = np.zeros((n_clusters, data.shape[1]))
			count_centers = np.zeros(n_clusters)
			iters += 1
		return inertia, centroids, membership

	best_inertia = np.inf
	best_centroids = np.zeros((n_clusters, data.shape[1]))
	best_labels = np.zeros(data.shape[0])
	loops = 0

	while loops < best_of:
		inertia, centroids, labels = _kmeans(data, n_clusters, init, tol, max_iter)
		print centroids
		if inertia < best_inertia:
			best_inertia = inertia
			best_centroids = centroids
			best_labels = labels
		loops += 1
	return best_inertia, best_centroids, best_labels

from sklearn.datasets import make_blobs
data, y = make_blobs(n_features=100, centers=5, n_samples=10000)
inertia, centers, labels = kmeans(data, best_of=1, max_iter=300, n_clusters=5, tol=0.0001)


