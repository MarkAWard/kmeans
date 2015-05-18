import numpy as np
import time

def distance(x1, x2):
	return np.sum((x1-x2)**2)

def find_nearest_centroid(point, centroids, distance=distance):
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
		if inertia < best_inertia:
			best_inertia = inertia
			best_centroids = centroids
			best_labels = labels
		loops += 1
	return best_inertia, best_centroids, best_labels


if __name__ == "__main__":
	import argparse

	parser = argparse.ArgumentParser(description="K-means algorithm")
	parser.add_argument('-f', dest='filename', type=str, 
						help="file containing data to be clustered")
	parser.add_argument('-k', dest='n_centroids', type=int, default=5,
						help="the number of clusters to find")
	parser.add_argument('-s', dest='sep', type=str, default=',',
						help="file delimiter")
	parser.add_argument('-n', dest='n_points', type=int, default=-1,
						help="number of points in file to read (optional)")
	parser.add_argument('-d', dest='dimensions', type=int, default=-1,
						help="fnumber of dimensions of input data (optional)")
	parser.add_argument('-e', dest='tol', type=float, default=1e-5,
						help="minimum fraction of points that don't change clusters to end kmeans loop")
	parser.add_argument('-i', dest='max_iter', type=int, default=100,
						help="maximum number of iterations within kmeans")
	parser.add_argument('-t', dest='trials', type=int, default=25,
						help="umber of kmeans trials to perform to find best clustering")
	parser.add_argument('-v', dest='verbose', type=int, default=0,
						help="control amount of printing 0, 1, 2 ")

	args = parser.parse_args()

	data = np.genfromtxt(args.filename, delimiter=args.sep)
	if args.n_points != -1:
		data = data[:args.n_points,:]
	if args.dimensions != -1:
		data = data[:,:args.dimensions]

	start = time.time()
	inertia, centers, labels = kmeans(data, best_of=args.trials, max_iter=args.max_iter, 
									n_clusters=args.n_centroids, tol=args.tol)
	end = time.time()

	print "\nPYTHON K-MEANS"
	print "%dx%d data, %d clusters, %d trials, 1 core" %(data.shape[0], data.shape[1], args.n_centroids, args.trials)
	print "Inertia: %f" %inertia
	print "Runtime: %f s" %(end - start)



