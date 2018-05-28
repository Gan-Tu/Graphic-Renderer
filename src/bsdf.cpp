#include "bsdf.h"

#include <iostream>
#include <algorithm>
#include <utility>

using std::min;
using std::max;
using std::swap;

namespace CGL {

void make_coord_space(Matrix3x3& o2w, const Vector3D& n) {

    Vector3D z = Vector3D(n.x, n.y, n.z);
    Vector3D h = z;
    if (fabs(h.x) <= fabs(h.y) && fabs(h.x) <= fabs(h.z)) h.x = 1.0;
    else if (fabs(h.y) <= fabs(h.x) && fabs(h.y) <= fabs(h.z)) h.y = 1.0;
    else h.z = 1.0;

    z.normalize();
    Vector3D y = cross(h, z);
    y.normalize();
    Vector3D x = cross(z, y);
    x.normalize();

    o2w[0] = x;
    o2w[1] = y;
    o2w[2] = z;
}

// Diffuse BSDF //

Spectrum DiffuseBSDF::f(const Vector3D& wo, const Vector3D& wi) {
  // This function takes in both wo and wi and returns the evaluation of
  // the BSDF for those two directions.
  return reflectance * (1.0 / PI);
}

Spectrum DiffuseBSDF::sample_f(const Vector3D& wo, Vector3D* wi, float* pdf) {
  // This function takes in only wo and provides pointers for wi and pdf,
  // which should be assigned by this function.
  // After sampling a value for wi, it returns the evaluation of the BSDF
  // at (wo, *wi).
  *wi = sampler.get_sample(pdf);
  return reflectance * (1.0 / PI);
}


// Mirror BSDF //

Spectrum MirrorBSDF::f(const Vector3D& wo, const Vector3D& wi) {
  return Spectrum();
}

Spectrum MirrorBSDF::sample_f(const Vector3D& wo, Vector3D* wi, float* pdf) {
  // TODO:
  // Implement MirrorBSDF
  BSDF::reflect(wo, wi);
  *pdf = 1;
  return reflectance / abs_cos_theta(wo);
}

// Microfacet BSDF //

double MicrofacetBSDF::G(const Vector3D& wo, const Vector3D& wi) {
    return 1.0 / (1.0 + Lambda(wi) + Lambda(wo));
}

double MicrofacetBSDF::D(const Vector3D& h) {
    // TODO: proj3-2, part 2
    // Compute Beckmann normal distribution function (NDF) here.
    // You will need the roughness alpha.

    double tan2 = sin_theta2(h) / (1 - sin_theta2(h));
    double deno = exp(-tan2/pow(alpha, 2));
    double num = PI * pow(alpha, 2) * pow(cos_theta(h), 4);

    return deno / num;
}

Spectrum MicrofacetBSDF::F(const Vector3D& wi) {
    // TODO: proj3-2, part 2
    // Compute Fresnel term for reflection on dielectric-conductor interface.
    // You will need both eta and etaK, both of which are Spectrum.

    // Cache intermediate coefficients
    Spectrum eta2_k2 = eta * eta + k * k;
    Spectrum two_eta_cos = 2 * eta * cos_theta(wi);
    double cos_theta2 = pow(cos_theta(wi), 2);

    Spectrum rs = (eta2_k2 - two_eta_cos + cos_theta2) / (eta2_k2 + two_eta_cos + cos_theta2);

    Spectrum rp = (eta2_k2 * cos_theta2- two_eta_cos + 1) / (eta2_k2 * cos_theta2 + two_eta_cos + 1);

    return (rs + rp) / 2;
}

Spectrum MicrofacetBSDF::f(const Vector3D& wo, const Vector3D& wi) {
    // TODO: proj3-2, part 2
    // Implement microfacet model here.

    Vector3D n = Vector3D(0,0,1);
    if (dot(n, wo) <= 0 || dot(n, wi) <= 0) {
      return Spectrum();
    }

    Vector3D h = (wi + wo).unit();

    Spectrum result = (MicrofacetBSDF::F(wi) * MicrofacetBSDF::G(wo, wi) * MicrofacetBSDF::D(h)) / (4 * dot(n, wo) * dot(n, wi));

    return result;
}

Spectrum MicrofacetBSDF::sample_f(const Vector3D& wo, Vector3D* wi, float* pdf) {
    // TODO: proj3-2, part 2
    // *Importance* sample Beckmann normal distribution function (NDF) here.
    // Note: You should fill in the sampled direction *wi and the corresponding *pdf,
    //       and return the sampled BRDF value.

    // *wi = cosineHemisphereSampler.get_sample(pdf); //placeholder
    // return MicrofacetBSDF::f(wo, *wi);

    Vector2D r = sampler.get_sample();
    double r1 = r.x, r2 = r.y;

    double tmp = -pow(alpha, 2) * log(1-r1);
    if (tmp < 0) {
      *pdf = 0;
      return Spectrum();
    }
    double theta = atan(sqrt(tmp));
    double phi = 2 * PI * r2;

    Vector3D h = Vector3D(sin(theta) * cos(phi),
                          sin(theta) * sin(phi),
                          cos(theta));
    
    *wi = -wo + 2 * dot(wo, h) * h;

    Vector3D n = Vector3D(0,0,1);
    if (dot(n, *wi) <= 0) {
      *pdf = 0;
      return Spectrum();
    }

    double p_theta_h = (2 * sin_theta(h)) / (pow(alpha, 2) * pow(cos_theta(h), 3)) * exp(-sin_theta2(h) / pow(cos_theta(h), 2) / pow(alpha, 2));

    double p_phi_h = 1 / (2 * PI);

    double pdf_h = p_theta_h * p_phi_h / sin_theta(h);

    *pdf = pdf_h / (4 * dot(*wi, h));

    return MicrofacetBSDF::f(wo, *wi);
}

// Refraction BSDF //

Spectrum RefractionBSDF::f(const Vector3D& wo, const Vector3D& wi) {
  return Spectrum();
}

Spectrum RefractionBSDF::sample_f(const Vector3D& wo, Vector3D* wi, float* pdf) {

  // TODO:
  // Implement RefractionBSDF
  return Spectrum();
}

// Glass BSDF //

Spectrum GlassBSDF::f(const Vector3D& wo, const Vector3D& wi) {
  return Spectrum();
}

Spectrum GlassBSDF::sample_f(const Vector3D& wo, Vector3D* wi, float* pdf) {
  // TODO: 3-2 Part 1 Task 4
  // Compute Fresnel coefficient and either reflect or refract based on it.
  // refract
  bool total_internal_reflection = !BSDF::refract(wo, wi, ior);
  if (total_internal_reflection) {
    BSDF::reflect(wo, wi);
    *pdf = 1;
    return reflectance / abs_cos_theta(*wi);
  }

  // Schlick's approximation
  float no = 1, ni = ior;
  if (wo.z < 0) {
    no = ior;
    ni = 1;
  }

  float r0 = pow((ni-no)/(ni+no), 2);
  float r = r0 + (1-r0) * pow(1 - abs_cos_theta(wo), 5);

  if (coin_flip(r)) {
    BSDF::reflect(wo, wi);
    *pdf = r;
    return r * reflectance / abs_cos_theta(*wi);
  } else {
    BSDF::refract(wo, wi, ior);
    *pdf = 1 - r;
    float eta = ior  / 1.0f;
    if (wo.z >= 0) {
      eta = 1.0f / ior;
    }
    return (1-r) * transmittance / pow(eta, 2) / abs_cos_theta(*wi);
  }
}


void BSDF::reflect(const Vector3D& wo, Vector3D* wi) {
  // TODO: 3-2 Part 1 Task 1
  // Implement reflection of wo about normal (0,0,1) and store result in wi.
  Vector3D n = Vector3D(0,0,1);
  *wi = -wo + 2 * dot(wo, n) * n;
}

bool BSDF::refract(const Vector3D& wo, Vector3D* wi, float ior) {
  // TODO: 3-2 Part 1 Task 3
  // Use Snell's Law to refract wo surface and store result ray in wi.
  // Return false if refraction does not occur due to total internal reflection
  // and true otherwise. When wo.z is positive, then wo corresponds to a
  // ray entering the surface through vacuum.

  // Calculate eta
  float eta = ior / 1.0f;
  if (wo.z >= 0) {
    eta = 1.0f / ior;
  }

  // Check if we have total internal reflection
  float wi_z_2 = 1 - pow(eta, 2) * sin_theta2(wo);
  if (wi_z_2 < 0) {
    return false;
  }

  wi->x = -eta * wo.x;
  wi->y = -eta * wo.y;
  wi->z = -sqrt(1 - pow(eta, 2) * (1 - pow(wo.z, 2)));
  if (wo.z < 0) {
    wi->z = -wi->z;
  }
  return true;
}

// Emission BSDF //

Spectrum EmissionBSDF::f(const Vector3D& wo, const Vector3D& wi) {
  return Spectrum();
}

Spectrum EmissionBSDF::sample_f(const Vector3D& wo, Vector3D* wi, float* pdf) {
  *pdf = 1.0 / PI;
  *wi  = sampler.get_sample(pdf);
  return Spectrum();
}

} // namespace CGL
