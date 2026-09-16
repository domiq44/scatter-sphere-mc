# 📐 Modèle de Simulation d'Impacts : Uniformité sur la Projection Inclinée

## 🎯 Objectif de la Simulation
L'objectif est de générer des points d'impact sur une sphère de rayon $R$, en simulant l'effet d'un flux de photons. Nous conservons l'hypothèse que la distribution des impacts est uniformément répartie sur la **projection de la sphère**, mais cette projection est définie par un plan incliné faisant un angle $\theta_0$ avec le plan de référence initial ($XY$).

## 📐 Hypothèses Fondamentales
1.  **Uniformité de la Source :** La source est supposée uniforme dans le plan de projection ($XY'$).
2.  **Faisceau Incident :** Le faisceau est supposé être **parallèle** et faire un angle $\theta_0$ avec l'axe vertical ($Z$).
3.  **Cas Limite :** Le cas vertical ($\theta_0 = 0$) est traité comme le cas limite du modèle général.

## ⚙️ Méthodologie de Génération des Points (Algorithme Général)
Le processus se décompose en deux phases : la génération dans le repère de projection, puis la transformation dans le repère monde.

### PHASE 1 : Génération dans le Repère de Projection ($XY'$)
Nous générons les coordonnées $(\rho, \phi)$ en supposant une projection verticale.

1.  **Génération des Paramètres Aléatoires :**
    *   **$\rho$ (Rayon de projection) :** Tiré uniformément dans $[0, R]$.
    *   **$\phi$ (Angle dans le plan de projection) :** Tiré uniformément dans $[0, 2\pi]$.

2.  **Coordonnées dans le Plan de Projection ($XY'$):**
    Pour garantir la compatibilité avec les prévisualiseurs, nous encadrons ce système en bloc de code :
    ```
    x' = rho * cos(phi)
    y' = rho * sin(phi)
    z' = 0
    ```

### PHASE 2 : Transformation vers le Repère Orthonormé Initial $(XYZ)$
Pour obtenir les coordonnées $(x, y, z)$ dans le repère initial, nous appliquons une rotation de $\theta_0$ autour d'un axe (par exemple, l'axe $Y$).

**Formules de Transformation (Rotation de $\theta_0$ autour de l'axe $Y$) :**
$$
\begin{cases} x = x' \cos(\theta_0) + z' \sin(\theta_0) \\ y = y' \\ z = -x' \sin(\theta_0) + z' \cos(\theta_0) \end{cases}
$$

**Simplification grâce à $z' = 0$ :**
$$
\begin{cases} \mathbf{x = x' \cos(\theta_0)} \\ \mathbf{y = y'} \\ \mathbf{z = -x' \sin(\theta_0)} \end{cases}
$$

**En substituant $x'$ par son expression en $\rho$ et $\phi$ :**
$$
\begin{cases} \mathbf{x = (\rho \cos(\phi)) \cos(\theta_0)} \\ \mathbf{y = \rho \sin(\phi)} \\ \mathbf{z = -(\rho \cos(\phi)) \sin(\theta_0)} \end{cases}
$$

### Cas Limite : Incidence Verticale ($\theta_0 = 0$)
Lorsque $\theta_0 = 0$, $\cos(0) = 1$ et $\sin(0) = 0$. Les formules se simplifient naturellement à :
$$
\begin{cases} x = \rho \cos(\phi) \\ y = \rho \sin(\phi) \\ z = 0 \end{cases}
$$

## ⚖️ Conclusion et Portée du Modèle
Ce modèle général permet de simuler l'impact d'un faisceau incident à n'importe quel angle $\theta_0$ tout en garantissant que la **densité des points générés est uniforme dans le plan de projection incliné** ($XY'$).
