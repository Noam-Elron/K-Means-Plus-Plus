import sklearn
import matplotlib.pyplot as plt


def get_correct_circle(ax, center, radius):
        # Create an ellipse in point coordinates that will be transformed to data coordinates
        circ = plt.Ellipse((0, 0), width=radius * 2, height=radius * 2, fill=False, linestyle='--', color='black')
        
        # Get the transformations
        transData = ax.transData
        transScale = plt.transforms.ScaledTranslation(center[0], center[1], transData)
        
        # Apply the transformation
        circ.set_transform(plt.transforms.AffineDelta2D(transScale))
        
        return circ

def plot_bonus():
    data = sklearn.datasets.load_iris()
    inertias = []    

    for K in range(1, 11):
        kmeans = sklearn.cluster.KMeans(n_clusters=K, random_state=0, init='k-means++') 
        kmeans.fit(data.data)
        inertias.append(kmeans.inertia_)
    print(inertias)
    

    fig, ax = plt.subplots()
    ax.plot(range(1, 11), inertias)
    ax.set_ylabel('Average Dispersion')
    ax.set_xlabel('K')
    plt.title('Elbow Method for selection of optimal "K" clusters')
    ax.set_xticks(range(1, 11))


    elbow_x, elbow_y = 2, inertias[1]   


    fig.canvas.draw()  # This is needed to get the correct data ratio
    data_ratio = ax.get_data_ratio()
    ax.plot(elbow_x,elbow_y,'bo',fillstyle='none',markersize=25)

    # Add a dashed circle around the elbow point
    ax.annotate('Elbow Point', xy=(elbow_x, elbow_y), xytext=(elbow_x+2, elbow_y+5),
                arrowprops=dict(facecolor='black', 
                                linestyle='--', lw=1, arrowstyle='->'),
                fontsize=12, ha='center')

    # Set axis labels
    fig.savefig("./elbow.png")
    plt.show(block=True)


def main():
    plot_bonus()

if __name__ == "__main__":
    
    main()